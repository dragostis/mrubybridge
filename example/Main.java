import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

public class Main {
    public static void main(String[] args) throws IOException {
        MRubyState mRubyState = new MRubyState("ruby");

        mRubyState.loadClass(Example.class);

        mRubyState.loadMethods();

        mRubyState.executeStream(new FileInputStream("ruby/example.rb"));
        mRubyState.close();
    }
}
