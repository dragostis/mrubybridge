import java.io.IOException;
import java.io.InputStream;

public interface Context {
    InputStream get(String fileName) throws IOException;
}
