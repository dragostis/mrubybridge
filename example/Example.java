public class Example {
    public int i;

    public Example(int i) {
        this.i = i;
    }

    public int add(Example other) {
        return i + other.i;
    }
}