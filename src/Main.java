import java.io.File;
import java.io.IOException;

public class Main {
    public static void main(String[] args) {
        MRubyState mRubyState = new MRubyState();

        try {
            mRubyState.executeFile(new File("src/fun.rb"));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
