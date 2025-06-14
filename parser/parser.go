package parser

import (
	"clovis/lexer"
	"clovis/semantics"
	"fmt"
)

type ParserError struct {
	token lexer.Token
	msg   string
}

func NewParserError(token lexer.Token, msg string) *ParserError {
	return &ParserError{
		token: token,
		msg: msg,
	}
}

func (e *ParserError) Error() string {
	return fmt.Sprintf("Error at line %v at column %v at token %v\n\t%v", e.token.Line, e.token.Col, e.token.Type, e.msg)
}

type Parser struct {
	Stmts  []Statement
	Errors []error
	tokens []lexer.Token
	idx    int
}

func NewParser(tokens []lexer.Token) *Parser {
	return &Parser{
		Stmts: []Statement{},
		Errors: []error{},
		tokens: tokens,
		idx: 0,
	}
}

func (p *Parser) Parse() error {
	p.parseProgram()

	if errLen := len(p.Errors); errLen != 0 {
		return p.Errors[errLen - 1]
	}

	return nil
}

func (p *Parser) parseProgram() {
	p.Stmts = p.parseStatements()
}

func (p *Parser) parseStatements() []Statement {
	stmts := []Statement{}

	for p.idx < len(p.tokens) && !p.isAtEnd() {
		stmt, err := p.parseStatement()
		if err != nil {
			p.Errors = append(p.Errors, err)
			p.synchronize()
			continue
		}
		stmts = append(stmts, stmt)
	}

	return stmts
}

func (p *Parser) parseStatement() (Statement, error) {
	if p.match(lexer.UINT) || p.match(lexer.BOOL) {
		return p.parseVarDecl()
	} else if p.match(lexer.IDENT) {
		return p.parseVarDefinition()
	} else if p.match(lexer.OPEN_CURLY) {
		return p.parseBlockStmt()
	} else if p.match(lexer.IF) {
		p.parseIfStmt()
	} else if p.match(lexer.WHILE) {
		p.parseWhileStmt()
	} else if p.match(lexer.FOR) {
		p.parseForStmt()
	} else if p.match(lexer.ASSERT) {
		return p.parseAssert()
	} else {
		return p.parseExpressionStmt()
	}

	return nil, nil
}

// <varDecl> ::= ( "uint" | "bool" ) ident ( ";" | "=" <expression> ";" )
func (p *Parser) parseVarDecl() (*VarDeclStmt, error) {
	varDeclStmt := &VarDeclStmt{}
	varTypeToken := p.consume()
	varDeclStmt.VarType = p.getType(varTypeToken.Type)

	if p.match(lexer.IDENT) {
		varDeclStmt.Ident = p.consume()
	} else {
		err := NewParserError(
			p.consume(),
			fmt.Sprintf("Expected an identifier after %v", varTypeToken.Type),
		)
		return nil, err
	}
	
	if p.match(lexer.ASSIGN) {
		p.consume()
		value, err := p.parseExpression()
		if err != nil {
			return nil, err
		}
		varDeclStmt.Value.SetVal(value)
	}

	if !p.match(lexer.SEMI) {
		err := NewParserError(
			p.peek(),
			fmt.Sprintf("Expected ';' after variable declaration"),
		)
		return nil, err
	} else {
		p.consume()
	}
	
	return varDeclStmt, nil
}

// <varDefinition> ::= ident "=" <expression> ";"
func (p *Parser) parseVarDefinition() (Statement, error) {
	ident := p.consume()

	if !p.match(lexer.ASSIGN) {
		err := NewParserError(
			p.peek(),
			"Expected '=' after variable identifier",
		)
		return nil, err
	}

	p.consume() // consume '='

	exprVal, err := p.parseExpression()
	if err != nil {
		e := NewParserError(
			p.peek(),
			fmt.Sprintf("At variable definition\n\t%v", err.Error()),
		)
		return nil, e
	}

	p.consume() // consume ';'

	varDefStmt := VarDefinitionStmt{
		Ident: ident,
		Value: exprVal,
	}
	
	return &varDefStmt, nil
}

// <blockStmt> ::= "{" <statements> "}"
func (p *Parser) parseBlockStmt() (Statement, error) {
	p.consume() // '{'

	stmts := []Statement{}

	for p.idx < len(p.tokens) && !p.isAtEnd() && !p.match(lexer.CLOSE_CURLY) {
		stmt, err := p.parseStatement()
		if err != nil {
			p.Errors = append(p.Errors, err)
			p.synchronize()
			continue
		}
		stmts = append(stmts, stmt)
	}

	blockStmt := BlockStmt{
		Statements: stmts,
	}
	
	if !p.match(lexer.CLOSE_CURLY) {
		err := NewParserError(
			p.peek(),
			fmt.Sprintf("Expected '}' but found '%v'", p.peek().Value),
		)
		return nil, err
	} else {
		p.consume() // '}'
	}

	return &blockStmt, nil
}

func (p *Parser) parseIfStmt() {

}

func (p *Parser) parseWhileStmt() {

}

func (p *Parser) parseForStmt() {

}

func (p *Parser) parseAssert() (Statement, error) {
	stmt := AssertStmt{}
	stmt.AssertToken = p.consume()

	expr, err := p.parseExpression()
	if err != nil {
		e := NewParserError(
			p.peek(),
			fmt.Sprintf("At assertion %v", err.Error()),
		)
		return nil, e
	}
	stmt.Expr = expr

	if !p.match(lexer.SEMI) {
		e := NewParserError(
			p.peek(),
			fmt.Sprintf("Expected ';' found %v", p.peek().Value),
		)
		return nil, e
	}
	p.consume() // ';'

	return &stmt, nil
}

func (p *Parser) parseExpressionStmt() (Statement, error) {
	exprStmt := ExpressionStmt{}

	expr, err := p.parseExpression()
	if err != nil {
		return nil, err
	}
	exprStmt.Expr = expr

	if !p.match(lexer.SEMI) {
		e := NewParserError(
			p.peek(),
			fmt.Sprintf("Expected ';' found %v", p.peek().Value),
		)
		return nil, e
	}
	p.consume() // ';'

	return &exprStmt, nil
}

// <expression> ::= <equality>
func (p *Parser) parseExpression() (Expression, error) {
	return p.parseEquality()
}

// <equality> ::= <comparison> { ("==" | "!=") <comparison> }
func (p *Parser) parseEquality() (Expression, error) {
	left, err := p.parseComparison()
	if err != nil {
		return nil, err
	}
	
	if p.matchAny(lexer.EQ, lexer.NEQ) {
		op := p.consume()
		right, err := p.parseComparison()
		if err != nil {
			return nil, err
		}

		left = &BinaryExpression{
			Type: semantics.UNKNOWN,
			Left: left,
			Op: op,
			Right: right,
		}
	}

	return left, nil
}

// <comparison> ::= <term> { ("<" | "<=" | ">" | ">=") <term> }
func (p *Parser) parseComparison() (Expression, error) {
	left, err := p.parseTerm()
	if err != nil {
		return nil, err
	}

	for p.matchAny(lexer.LESS_THAN, lexer.LESS_EQ_THAN, lexer.GREATER_THAN, lexer.GREATER_EQ_THAN) {
		op := p.consume()
		right, err := p.parseTerm()
		if err != nil {
			return nil, err
		}

		left = &BinaryExpression{
			Type: semantics.UNKNOWN,
			Left: left,
			Op: op,
			Right: right,
		}
	}

	return left, nil
}

// <term> ::= <factor> { ("+" | "-") <factor> }
func (p *Parser) parseTerm() (Expression, error) {
	left, err := p.parseFactor()
	if err != nil {
		return nil, err
	}

	for p.matchAny(lexer.PLUS, lexer.MINUS) {
		op := p.consume()
		right, err := p.parseFactor()
		if err != nil {
			return nil, err
		}

		left = &BinaryExpression{
			Type: semantics.UNKNOWN,
			Left: left,
			Op: op,
			Right: right,
		}
	}

	return left, nil
}

// <factor> ::= <unary> { ("*" | "/") <unary> }
func (p *Parser) parseFactor() (Expression, error) {
	left, err := p.parseUnary()
	if err != nil {
		return nil, err
	}

	for p.matchAny(lexer.STAR, lexer.F_SLASH) {
		op := p.consume()
		right, err := p.parseUnary()
		if err != nil {
			return nil, err
		}

		left = &BinaryExpression{
			Type: semantics.UNKNOWN,
			Left: left,
			Op: op,
			Right: right,
		}
	}

	return left, nil
}

// <unary> ::= ( "!" | "-" ) <unary> | <primary>
func (p *Parser) parseUnary() (Expression, error) {
	if p.matchAny(lexer.NOT, lexer.MINUS) {
		op := p.consume()
		right, err := p.parseUnary()
		if err != nil {
			return nil, err
		}

		un := &UnaryExpression{
			Type: semantics.UNKNOWN,
			Op: op,
			Right: right,
		}

		return un, nil
	}

	return p.parsePrimary()
}

// <primary> ::= <literal> | ident | "(" <expression> ")" 
func (p *Parser) parsePrimary() (Expression, error) {
	if p.matchAny(lexer.UINT_LIT, lexer.TRUE_LIT, lexer.FALSE_LIT) {
		litExpr := &LiteralExpression{
			Type: p.getType(p.peek().Type),
			Value: p.consume(),
		}
		return litExpr, nil
	} else if p.match(lexer.IDENT) {
		identExpr := &IdentExpression{
			Type: semantics.UNKNOWN,
			Ident: p.consume(),
		}
		return identExpr, nil
	} else if p.match(lexer.OPEN_PAREN) {
		return p.parseGroupExpr()
	} else {
		err := NewParserError(
			p.peek(),
			"Invalid expression",
		)
		return nil, err
	}
}

// <groupExpr> ::= "(" <expression> ")"
func (p *Parser) parseGroupExpr() (Expression, error) {
	groupExpr := &GroupExpression{
		Type: semantics.UNKNOWN,
	}

	p.consume()

    expr, err := p.parseExpression()
    if err != nil {
    	return nil, err
    }

	groupExpr.Expr = expr
    
    if !p.match(lexer.CLOSE_PAREN) {
    	err = NewParserError(
        	p.consume(),
        	"Expected ')' after group expression",
    	)

    	return nil, err
    }

    p.consume()

    return groupExpr, nil
}

func (p *Parser) consume() lexer.Token {
	t := p.tokens[p.idx]
	p.idx++
	return t
}

func (p *Parser) peek() lexer.Token {
	t := p.tokens[p.idx]
	return t
}

func (p *Parser) match(tokenType lexer.TokenType) bool {
	return p.tokens[p.idx].Type == tokenType
}

func (p *Parser) matchAny(tokenTypes ...lexer.TokenType) bool {
	for _, t := range tokenTypes {
		if p.tokens[p.idx].Type == t {
			return true
		}
	}

	return false
}

func (p *Parser) isAtEnd() bool {
	return p.tokens[p.idx].Type == lexer.EOF
}

func (p *Parser) synchronize() {
	for !p.isAtEnd() && p.matchAny(lexer.SEMI, lexer.CLOSE_CURLY, lexer.EOF) {
		p.consume()
	}

	p.consume()
}

func (p *Parser) getType(tokenType lexer.TokenType) semantics.Type {
	switch tokenType {
	case lexer.UINT:
		fallthrough
	case lexer.UINT_LIT:
		return semantics.UINT
	case lexer.BOOL:
		fallthrough
	case lexer.TRUE_LIT:
		fallthrough
	case lexer.FALSE_LIT:
		return semantics.BOOL
	}

	return semantics.UNKNOWN
}
