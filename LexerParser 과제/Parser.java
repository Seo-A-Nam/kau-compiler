import java.sql.Types;
import java.util.*;

import javax.naming.TimeLimitExceededException;

public class Parser {
    // Recursive descent parser that inputs a C++Lite program and 
    // generates its abstract syntax.  Each method corresponds to
    // a concrete syntax grammar rule, which appears as a comment
    // at the beginning of the method.
  
    Token token;          // current token from the input stream
    Lexer lexer;
  
    public Parser(Lexer ts) { // Open the C++Lite source program
        lexer = ts;                          // as a token stream, and
        token = lexer.next();            // retrieve its first Token
    }
  
    private String match (TokenType t) { // * return the string of a token if it matches with t *
        String value = token.value();
        if (token.type().equals(t))
            token = lexer.next();
        else
            error(t);
        return value;
    }
  
    private void error(TokenType tok) {
        System.err.println("Syntax error: expecting: " + tok 
                           + "; saw: " + token);
        System.exit(1);
    }
    
    private void error(String tok) {
        System.err.println("Syntax error: expecting: " + tok 
                           + "; saw: " + token);
        System.exit(1);
    }
    
    public Program program() {
        // Program --> void main ( ) '{' Declarations Statements '}'
        TokenType[ ] header = {TokenType.Int, TokenType.Main,
                          TokenType.LeftParen, TokenType.RightParen};

        for (int i=0; i<header.length; i++)   // bypass "int main ( )"
            match(header[i]);
        match(TokenType.LeftBrace);
        Declarations dcs = declarations();
        Block b = statements();
        // student exercise
        match(TokenType.RightBrace);
        return (new Program(dcs, b));  // student exercise
    }
  
    private Declarations declarations () {
        // Declarations --> { Declaration }
        Declarations ds = new Declarations();
        while (isType())
            declaration(ds);
        return (ds);  // student exercise
    }
  
    private void declaration (Declarations ds) {
        // Declaration  --> Type Identifier { , Identifier } ;
        Type t = type();
        Variable v = new Variable(match(TokenType.Identifier));
        Declaration d = new Declaration(v, t);
        ds.add(d);
        
        while (isComma()) {
            token = lexer.next();
            v = new Variable(match(TokenType.Identifier));
            d = new Declaration(v, t);
        }
        match(TokenType.Semicolon);
        // student exercise
    }
  
    private Type type () {
        // Type  -->  int | bool | float | char 
        Type t = null;
        if (token.type().equals(TokenType.Int))
            t = Type.INT;
        else if (token.type().equals(TokenType.Bool))
            t = Type.BOOL;
        else if (token.type().equals(TokenType.Float))
            t = Type.FLOAT;
        else if (token.type().equals(TokenType.Char))
            t = Type.CHAR;
        else {
            error("[error] type");
        }
        token = lexer.next();
        // student exercise
        return t;          
    }
  
    private Statement statement() {
        // Statement --> ; | Block | Assignment | IfStatement | WhileStatement
        Statement s = null;
        
        if (token.type().equals(TokenType.Semicolon))
            s = new Skip();
        else if (token.type().equals(TokenType.LeftBrace))
            s = statements();
        else if (token.type().equals(TokenType.If))
            s = ifStatement();
        else if (token.type().equals(TokenType.While))
            s = whileStatement();
        else if (token.type().equals(TokenType.Identifier))
            s = assignment();
        else error("[error] statement");
        // student exercise
        return s;
    }
  
    private Block statements () {
        // Block --> '{' Statements '}'
        Block b = new Block();
        Statement s;
        
        //match(TokenType.LeftBrace);
        while (isStatement()) { // statement가 맞다면
            s = statement();
            b.members.add(s); // arraylist에 추가
        }
        //match(TokenType.RightBrace);
        // student exercise
        return b;
    }
  
    private Assignment assignment () {
        // Assignment --> Identifier = Expression ;\
        Variable target = new Variable(match(TokenType.Identifier));
        match(TokenType.Assign);
        Expression src = expression();
        match(TokenType.Semicolon);
        return (new Assignment(target, src));  // student exercise
    }
  
    private Conditional ifStatement () {
        // IfStatement --> if ( Expression ) Statement [ else Statement ]
        Conditional con;
        
        match(TokenType.If);
        match(TokenType.LeftParen);
        Expression exp = expression();
        match(TokenType.RightParen);
        match(TokenType.LeftBrace);
        Statement s = statement();
        match(TokenType.RightBrace);
        if (token.type().equals(TokenType.Else)) {
            token = lexer.next();
            Statement estate = statement();
            con = new Conditional(exp, s, estate);
        } else {
            con = new Conditional(exp, s);
        }
        return (con);  // student exercise
    }
  
    private Loop whileStatement () {
        // WhileStatement --> while ( Expression ) Statement
        match(TokenType.While);
        match(TokenType.LeftParen);
        Expression exp = expression();
        match(TokenType.RightParen);
        Statement s = statement();
        return (new Loop(exp, s));  // student exercise
    }

    private Expression expression () {
        // Expression --> Conjunction { || Conjunction }
        Expression cj = conjunction();
        
        while (token.type().equals(TokenType.Or)) {
            Operator op = new Operator(match(token.type()));
            Expression ep = expression();
            cj = new Binary(op, cj, ep);
        }
        return (cj);  // student exercise
    }
  
    private Expression conjunction () {
        // Conjunction --> Equality { && Equality }
        Expression eq = equality();
        while (token.type().equals(TokenType.And)) {
            Operator op = new Operator(match(token.type()));
            Expression cj = conjunction();
            eq = new Binary(op, eq, cj);
        }
        return (eq);  // student exercise
    }
  
    private Expression equality () {
        // Equality --> Relation [ EquOp Relation ]
        Expression r = relation();
        while (isEqualityOp()) {
            Operator op = new Operator(match(token.type()));
            Expression r2 = relation();
            r = new Binary(op, r, r2);
        }
        return (r);  // student exercise
    }

    private Expression relation (){
        // Relation --> Addition [RelOp Addition] 
        Expression ad = addition();
        while (isRelationalOp()) {
            Operator op = new Operator(match(token.type()));
            Expression ad2 = addition();
            ad = new Binary(op, ad, ad2);
        }
        return (ad);  // student exercise
    }
  
    private Expression addition () {
        // Addition --> Term { AddOp Term }
        Expression e = term();
        while (isAddOp()) {
            Operator op = new Operator(match(token.type()));
            Expression term2 = term();
            e = new Binary(op, e, term2);
        }
        return e;
    }
  
    private Expression term () {
        // Term --> Factor { MultiplyOp Factor }
        Expression e = factor();
        while (isMultiplyOp()) {
            Operator op = new Operator(match(token.type()));
            Expression term2 = factor();
            e = new Binary(op, e, term2);
        }
        return e;
    }
  
    private Expression factor() {
        // Factor --> [ UnaryOp ] Primary 
        if (isUnaryOp()) {
            Operator op = new Operator(match(token.type()));
            Expression term = primary();
            return new Unary(op, term);
        }
        else return primary();
    }
  
    private Expression primary () {
        // Primary --> Identifier | Literal | ( Expression )
        //             | Type ( Expression )
        Expression e = null;
        if (token.type().equals(TokenType.Identifier)) {
            e = new Variable(match(TokenType.Identifier));
        } else if (isLiteral()) {
            e = literal();
        } else if (token.type().equals(TokenType.LeftParen)) {
            token = lexer.next();
            e = expression();       
            match(TokenType.RightParen);
        } else if (isType( )) {
            Operator op = new Operator(match(token.type()));
            match(TokenType.LeftParen);
            Expression term = expression();
            match(TokenType.RightParen);
            e = new Unary(op, term);
        } else error("Identifier | Literal | ( | Type");
        return e;
    }

    private Value literal( ) {
        Value val = null;
        String str = token.value();
        
        if (token.type().equals(TokenType.IntLiteral)) {
            val = new IntValue(Integer.parseInt(str)); // integer.parseint -- atoi랑 같은 기능
            token = lexer.next();
        } else if (token.type().equals(TokenType.FloatLiteral)) {
            val = new FloatValue(Float.parseFloat(str));
            token = lexer.next();
        } else if (token.type().equals(TokenType.CharLiteral)) {
            val = new CharValue(str.charAt(0)); // charAt() -- string의 특정 인덱스를 char로 리턴
            token = lexer.next();
        } else if (token.type().equals(TokenType.True)) {
            val = new BoolValue(true);
            token = lexer.next();
        } else error("[error] literal value");
        return (val);  // student exercise
    }
  

    private boolean isAddOp( ) {
        return token.type().equals(TokenType.Plus) ||
               token.type().equals(TokenType.Minus);
    }
    
    private boolean isMultiplyOp( ) {
        return token.type().equals(TokenType.Multiply) ||
               token.type().equals(TokenType.Divide);
    }
    
    private boolean isUnaryOp( ) {
        return token.type().equals(TokenType.Not) ||
               token.type().equals(TokenType.Minus);
    }
    
    private boolean isEqualityOp( ) {
        return token.type().equals(TokenType.Equals) ||
            token.type().equals(TokenType.NotEqual);
    }
    
    private boolean isRelationalOp( ) {
        return token.type().equals(TokenType.Less) ||
               token.type().equals(TokenType.LessEqual) || 
               token.type().equals(TokenType.Greater) ||
               token.type().equals(TokenType.GreaterEqual);
    }
    
    private boolean isType( ) {
        return token.type().equals(TokenType.Int)
            || token.type().equals(TokenType.Bool) 
            || token.type().equals(TokenType.Float)
            || token.type().equals(TokenType.Char);
    }
    
    private boolean isLiteral( ) {
        return token.type().equals(TokenType.IntLiteral) ||
            isBooleanLiteral() ||
            token.type().equals(TokenType.FloatLiteral) ||
            token.type().equals(TokenType.CharLiteral);
    }
    
    private boolean isBooleanLiteral( ) {
        return token.type().equals(TokenType.True) ||
            token.type().equals(TokenType.False);
    }
    
    private boolean isComma( ) {
        return token.type().equals(TokenType.Comma);
    }
   
    private boolean isSemicolon( ) {
        return token.type().equals(TokenType.Semicolon);
    }

    private boolean isLeftBrace() {
        return token.type().equals(TokenType.LeftBrace);
    } 
 
    private boolean isRightBrace() {
        return token.type().equals(TokenType.RightBrace);
    }
    
    private boolean isStatement() {
        return 	isSemicolon() ||
            isLeftBrace() ||
            token.type().equals(TokenType.If) ||
            token.type().equals(TokenType.While) ||
            token.type().equals(TokenType.Identifier); 
    }
    
    public static void main(String args[]) {
        //Parser parser  = new Parser(new Lexer(args[0]));
        Parser parser  = new Parser(new Lexer("input.txt"));
        Program prog = parser.program();
    } //main

} // Parser
