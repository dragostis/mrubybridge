import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.HashMap;

public class UnixContext implements Context {
    private HashMap<String, File> streams = new HashMap<String, File>();

    public UnixContext(String rootPath) {
        recurse(new File(rootPath), "");
    }

    @Override
    public InputStream get(String fileName) throws FileNotFoundException {
        return new FileInputStream(streams.get(fileName));
    }

    private void recurse(File root, String path) {
        if (path.endsWith(".rb")) {
            streams.put(path.substring(0, path.length() - 3), root);
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
