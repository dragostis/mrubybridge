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
        thrown.expect(RuntimeException.class);
        thrown.expectMessage("SyntaxError: line 2: syntax error, unexpected keyword_end, expecting $end");

        File file = loader.getFile("ruby/incorrect_syntax.rb");
        state.executeFile(file);
    }
}