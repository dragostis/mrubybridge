import org.junit.*;
import org.junit.rules.ExpectedException;
import resources.helper.Loader;

import java.io.IOException;
import java.io.InputStream;

import static org.junit.Assert.fail;

public class MRubyStateTest {
    private MRubyState state;
    private static Loader loader;

    @BeforeClass
    public static void loader() {
        loader = Loader.getInstance();
    }

    @Before
    public void setUp() {
        state = new MRubyState(loader.getPath("ruby"));
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

        InputStream inputStream = loader.getInputStream("ruby/syntax_error.rb");
        state.executeStream(inputStream, "syntax_error.rb");
    }

    @Test
    public void testExecuteErrorHandlingType() throws IOException {
        thrown.expect(RuntimeException.class);
        thrown.expectMessage("type_error.rb:1: non float value (TypeError)");

        InputStream inputStream = loader.getInputStream("ruby/type_error.rb");
        state.executeStream(inputStream, "type_error.rb");
    }

    @Test
    public void testRequire() throws IOException {
        try {
            InputStream inputStream = loader.getInputStream("ruby/default.rb");
            state.executeStream(inputStream, "default.rb");
        } catch (RuntimeException e) {
            fail(e.getMessage());
        }
    }
}