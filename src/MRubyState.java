import java.io.*;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.*;

public class MRubyState {
    static {
        System.loadLibrary("mrubystate");
    }

    public long pointer;
    private List<Class> classes;
    private Context context;
    private File current;
    private RequiredFiles requiredFiles = new RequiredFiles();

    public MRubyState(File rootDirectory) {
        pointer = getStatePointer();
        classes = new ArrayList<Class>();
        context = new Context(rootDirectory);
    }

    public void loadClass(Class aClass) {
        classes.add(aClass);

        loadClassToState(pointer, getJavaClassName(aClass), getRubyClassName(aClass));
    }

    public void loadMethods() {
        Method[] publicMethods;
        Constructor[] constructors;

        for (Class aClass : classes) {
            publicMethods = filterObjectMethods(aClass.getMethods());
            constructors = aClass.getConstructors();

            int length = publicMethods.length + constructors.length;

            String[] javaNames = new String[length];
            String[] rubyNames = new String[length];
            String[] javaSignatures = new String[length];
            boolean[] isStatic = new boolean[length];

            for (int i = 0; i < constructors.length; i++) {
                javaNames[i] = "<init>";
                rubyNames[i] = "construct";
                javaSignatures[i] = getConstructorSignature(constructors[i]);
                isStatic[i] = false;
            }

            for (int i = 0; i < publicMethods.length; i++) {
                int j = i + constructors.length;

                javaNames[j] = publicMethods[i].getName();
                rubyNames[j] = getRubyMethodName(javaNames[j]);
                javaSignatures[j] = getSignature(publicMethods[i]);
                isStatic[j] = Modifier.isStatic(publicMethods[i].getModifiers());
            }

            loadClassMethodsToState(pointer, getJavaClassName(aClass), getRubyClassName(aClass), javaNames, rubyNames,
                    javaSignatures, isStatic);
        }
    }

    public void close() {
        close(pointer);
    }

    private String getJavaClassName(Class aClass) {
        return aClass.getName().replaceAll("\\.", "/");
    }

    private String getRubyClassName(Class aClass) {
        return aClass.getName().replaceAll("(\\w+\\.)*", "");
    }

    private String getRubyMethodName(String javaName) {
        javaName = javaName.replaceAll("get", "");
        if (javaName.matches("set.*")) javaName = javaName.substring(3) + '=';

        javaName = Character.toLowerCase(javaName.charAt(0)) + javaName.substring(1);

        javaName = javaName.replaceAll("add", "+");
        javaName = javaName.replaceAll("subtract", "-");
        javaName = javaName.replaceAll("multiply", "*");
        javaName = javaName.replaceAll("divide", "/");

        return javaName.replaceAll("([A-Z])", "_$1").toLowerCase();
    }

    private String getConstructorSignature(Constructor constructor) {
        String signature = "(";

        for (Class aClass : constructor.getParameterTypes()) {
            signature += getClassTag(aClass);
        }

        signature += ")";

        return signature + "V";
    }

    private String getSignature(Method method) {
        String signature = "(";

        for (Class aClass : method.getParameterTypes()) {
            signature += getClassTag(aClass);
        }

        signature += ")";

        return signature + getClassTag(method.getReturnType());
    }

    private String getClassTag(Class aClass) {
        String tag = aClass.getCanonicalName();

        if (tag.endsWith("[]")) {
            tag = "[" + tag.substring(0, tag.length() - 2);
        }

        if (tag.startsWith("[") && !tag.startsWith("[L")) {
            tag = "[L" + tag.substring(1);
        } else if (!tag.startsWith("L")) {
            tag = "L" + tag;
        }

        if (!tag.endsWith(";")) {
            tag += ";";
        }

        tag = tag.replaceAll("L((java\\.lang\\.Boolean)|(boolean));", "Z");
        tag = tag.replaceAll("L((java\\.lang\\.Byte)|(byte));", "B");
        tag = tag.replaceAll("L((java\\.lang\\.Character)|(char));", "C");
        tag = tag.replaceAll("L((java\\.lang\\.Short)|(short));", "S");
        tag = tag.replaceAll("L((java\\.lang\\.Integer)|(int));", "I");
        tag = tag.replaceAll("L((java\\.lang\\.Long)|(long));", "J");
        tag = tag.replaceAll("L((java\\.lang\\.Float)|(float));", "F");
        tag = tag.replaceAll("L((java\\.lang\\.Double)|(double));", "D");

        tag = tag.replaceAll("Lvoid;", "V");

        return tag.replaceAll("\\.", "/");
    }

    private Method[] filterObjectMethods(Method[] methods) {
        List<Method> newMethods = new ArrayList<Method>();
        List<Method> toBeRemoved = new ArrayList<Method>();

        Collections.addAll(toBeRemoved, Object.class.getMethods());

        for (Method method : methods) {
            boolean toRemove = false;

            for (Method remove : toBeRemoved) {
                if (method.getName().equals(remove.getName())) toRemove = true;
            }

            if (!toRemove) newMethods.add(method);
        }

        Object[] objects = newMethods.toArray();
        Method[] result = new Method[objects.length];

        for (int i = 0; i < objects.length; i++) result[i] = (Method) objects[i];

        return result;
    }

    public void executeString(String mrubyString, String fileName) {
        loadString(pointer, mrubyString, fileName);
    }

    public void executeStream(InputStream mrubyStream, String fileName) throws IOException {
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(mrubyStream));
        StringBuilder stringBuilder = new StringBuilder();

        String line;

        while((line = bufferedReader.readLine()) != null) {
            stringBuilder.append(line);
            stringBuilder.append("\n");
        }

        loadString(pointer, stringBuilder.toString(), fileName);
    }

    private boolean require(String fileName) throws IOException {
        File mrubyFile = context.get(fileName);

        if (requiredFiles.contains(current, mrubyFile)) {
            return false;
        } else {
            File old = current;

            current = mrubyFile;
            executeFile(mrubyFile);
            current = old;

            return true;
        }
    }

    private static class RequiredFiles {
        private HashMap<File, HashSet<File>> required = new HashMap<File, HashSet<File>>();

        public boolean contains(File required, File file) {
            HashSet<File> files = this.required.get(required);

            if (files == null) {
                HashSet<File> set = new HashSet<File>();

                set.add(file);

                this.required.put(required, set);

                return false;
            } else {
                if (files.contains(file)) {
                    return true;
                } else {
                    files.add(file);

                    return false;
                }
            }
        }
    }

    private native long getStatePointer();

    private native void loadClassToState(long pointer, String javaName, String rubyName);

    private native void loadClassMethodsToState(long pointer, String className, String rubyClassName,
                                                String[] javaNames, String[] rubyNames, String[] javaSignatures,
                                                boolean[] isStatic);

    private native void loadString(long pointer, String mrubyString, String fileName);

    private native void close(long pointer);
}
