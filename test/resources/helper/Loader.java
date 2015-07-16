package resources.helper;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.net.URL;

public class Loader {
    private static Loader instance;
    private ClassLoader classLoader;

    private Loader() {
        this.classLoader = this.getClass().getClassLoader();
    }

    public static Loader getInstance() {
        if (instance == null) instance = new Loader();

        return instance;
    }

    public String getPath(String path) {
        return classLoader.getResource(path).getFile();
    }

    public FileInputStream getInputStream(String path) throws NullPointerException, FileNotFoundException {
        URL url = classLoader.getResource(path);

        return new FileInputStream(url.getFile());
    }
}
