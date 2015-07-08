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
        state = new MRubyState(loader.getFile("ruby"));
    }


    @After
    public void tearDown() {
        state.close();
    }

    @Rule
    public ExpectedException thrown = ExpectedException.none();



    @Test
    public void testExecuteErrorHandlingSyntax() throws IOException {
        thrown.expect(RuntimeException.class);
        thrown.expectMessage("SyntaxError: line 2: syntax error, unexpected keyword_end, expecting $end");

        File file = loader.getFile("ruby/syntax_error.rb");
        state.executeFile(file);
    }


    @Test
    public void testExecuteErrorHandlingType() throws IOException {
        thrown.expect(RuntimeException.class);
        thrown.expectMessage("type_error.rb:1: non float value (TypeError)");

        File file = loader.getFile("ruby/type_error.rb");
        state.executeFile(file);
    }
}