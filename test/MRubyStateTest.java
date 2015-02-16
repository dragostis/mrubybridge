import org.junit.*;
import org.junit.rules.ExpectedException;
import resources.helper.Loader;

import java.io.File;
import java.io.IOException;

public class MRubyStateTest {
    private MRubyState state;
    private static Loader loader;


    @BeforeClass
    public static void loader() {
        loader = Loader.getInstance();
    }


    @Before
    public void setUp() {
        state = new MRubyState();
    }


    @After
    public void tearDown() {
        // Finalize does not seem to work, null pointer exception
        try {
            state.finalize();
        } catch (Throwable throwable) {
            throwable.printStackTrace();
        }
    }

    @Rule
    public ExpectedException thrown = ExpectedException.none();



    @Test
    public void testExecute_errorHandling_incorrectFileSyntax() throws IOException {
        try {
            File file = loader.getFile("ruby/incorrect_syntax.rb");
            state.executeFile(file);
        } catch( RuntimeException e ) {
            System.out.println("Testing >>");
            System.out.print(e.getMessage());
            System.out.println();
        }
    }


    @Test(expected=IOException.class)
    public void testExecute_errorHandling_exception() throws IOException {
        File file = loader.getFile("ruby/exception.rb");
        state.executeFile(file);
    }
}