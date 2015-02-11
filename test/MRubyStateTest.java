import org.junit.*;

import java.io.File;
import java.io.IOException;

public class MRubyStateTest {
    private MRubyState state;

    @Before
    public void setUp() {
        state = new MRubyState();
    }

    @After
    public void tearDown() {
        // Finalize does not seem to work, null pointer exception
    }


    @Test(expected=RuntimeException.class)
    public void testExecute_incorrectFileSyntax_errorHandling() throws RuntimeException, IOException {
        ClassLoader classLoader = getClass().getClassLoader();
        File file = new File(classLoader.getResource("/ruby/incorrect_synteax.rb").getFile());

        state.executeFile(file);
    }
}