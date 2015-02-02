import java.io.*;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class MRubyState {
    static {
        System.loadLibrary("mrubystate");
    }

    private long pointer;
    private List<Class> classes;
    private List<Method> toBeRemoved;

    public MRubyState() {
        pointer = getStatePointer();

        classes = new ArrayList<Class>();

        toBeRemoved = new ArrayList<Method>();

        Collections.addAll(toBeRemoved, Object.class.getMethods());
    }

    public void loadClass(Class aClass) {
        classes.add(aClass);

        loadClassToState(pointer, aClass.getCanonicalName(), aClass.getName());
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
                rubyNames[j] = getRubyName(javaNames[j]);
                javaSignatures[j] = getSignature(publicMethods[i]);
                isStatic[j] = Modifier.isStatic(publicMethods[i].getModifiers());
            }

            loadClassMethodsToState(pointer, aClass.getCanonicalName(), aClass.getName(), javaNames, rubyNames,
                    javaSignatures, isStatic);
        }
    }

    private String getRubyName(String javaName) {
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

    public void executeFile(File mrubyFile) throws IOException {
        BufferedReader bufferedReader = new BufferedReader(new FileReader(mrubyFile));
        StringBuilder stringBuilder = new StringBuilder();

        String line;

        while((line = bufferedReader.readLine()) != null) {
            stringBuilder.append(line);
            stringBuilder.append("\n");
        }

        loadString(pointer, stringBuilder.toString(), mrubyFile.getName());
    }

    private native long getStatePointer();

    private native void loadClassToState(long pointer, String javaName, String rubyName);

    private native void loadClassMethodsToState(long pointer, String className, String rubyClassName,
                                                String[] javaNames, String[] rubyNames, String[] javaSignatures,
                                                boolean[] isStatic);

    private native void loadString(long pointer, String mrubyString, String fileName);
}
