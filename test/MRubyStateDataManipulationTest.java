import org.junit.*;
import org.junit.rules.ExpectedException;
import resources.Identity;
import resources.helper.Loader;

import java.io.IOException;
import java.io.InputStream;

import static org.junit.Assert.fail;

public class MRubyStateDataManipulationTest {
    private MRubyState state;
    private static Loader loader;


    @Rule
    public ExpectedException thrown = ExpectedException.none();


    @BeforeClass
    public static void loader() {
        loader = Loader.getInstance();
    }


    @Before
    public void setUp() throws IOException {
        state = new MRubyState(loader.getPath("ruby"));
        state.loadClass(Identity.class);
        state.loadMethods();
    }


    @After
    public void tearDown() {
        state.close();
    }


    @Test
    public void testExecuteDataReturnUnaltered() throws IOException {
        try {
            InputStream inputStream = loader.getInputStream("ruby/method_calls.rb");
            state.executeStream(inputStream, "method_calls.rb");
        } catch (RuntimeException e) {
            fail(e.getMessage());
        }
    }

    @Test
    public void testExecuteComplexSignature() throws IOException {
        try {
            InputStream inputStream = loader.getInputStream("ruby/complex_signature.rb");
            state.executeStream(inputStream, "complex_signature.rb");
        } catch (RuntimeException e) {
            fail(e.getMessage());
        }
    }
}
