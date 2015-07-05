import java.io.File;
import java.io.IOException;

public class Main {
    public static void main(String[] args) throws IOException {
        MRubyState mRubyState = new MRubyState();

        mRubyState.loadClass(Example.class);

        mRubyState.loadMethods();

        mRubyState.executeFile(new File("src/example.rb"));
        mRubyState.close();
    }
}
