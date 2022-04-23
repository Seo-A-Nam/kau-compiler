public class Rdp { // display abstract syntax tree
	public static void main(String args[]) {
        //Parser parser  = new Parser(new Lexer(args[0]));
        Parser parser  = new Parser(new Lexer("input.txt"));
        Program prog = parser.program();
        prog.display(0); // display abstract syntax tree
    } //main
}
