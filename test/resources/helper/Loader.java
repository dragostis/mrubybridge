package resources.helper;

import java.io.File;
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

    public File getFile(String path) throws NullPointerException {
        URL url = classLoader.getResource(path);

        return new File(url.getFile());
    }
}
