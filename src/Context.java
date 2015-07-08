import java.io.File;
import java.util.HashMap;

public class Context {
    private HashMap<String, File> files = new HashMap<String, File>();

    public Context(File root) {
        recurse(root, "");
    }

    public File get(String fileName) {
        return files.get(fileName);
    }

    private void recurse(File root, String path) {
        if (path.endsWith(".rb")) {
            files.put(path.substring(0, path.length() - 3), root);
        }

        if (root.isDirectory()) {
            for (File file : root.listFiles()) {
                String fullPath = file.getName();

                if (path.length() > 0) {
                    fullPath = path + "/" + fullPath;
                }

                recurse(file, fullPath);
            }
        }
    }
}
