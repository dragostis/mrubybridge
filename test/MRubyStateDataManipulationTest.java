import org.junit.*;
import org.junit.rules.ExpectedException;
import resources.Identity;
import resources.helper.Loader;

import java.io.File;
import java.io.IOException;

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
        state = new MRubyState();
        state.loadClass(Identity.class);
        state.loadMethods();
    }


    @After
    public void tearDown() {
        state.close();
    }


    @Test
    public void testExecute_dataReturn_unaltered() throws IOException {
        try {
            File file = loader.getFile("ruby/method_calls.rb");
            state.executeFile(file);
        } catch (RuntimeException e) {
            fail(e.getMessage());
        }
    }
}
