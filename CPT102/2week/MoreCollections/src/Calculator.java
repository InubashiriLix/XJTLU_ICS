import java.util.Stack;

public class Calculator {
    private Stack<Character> brancketStack = new Stack<>();

    public Calculator(String input) {
        // System.out.println("Input is " + (isBraBal(input) ? "yes" : "no"));
        System.out.println(transFix("1+2*3-4"));
    }

    private boolean isBraBal(final String input) {
        for (char ch : input.toCharArray()) {
            switch (ch) {
                // front
                case '(':
                case '{':
                case '[': { // Open brackets are pushed onto the stack
                    brancketStack.push(ch);
                    break;
                }

                // end
                case ')':
                case ']':
                case '}': {
                    if (brancketStack.isEmpty()) {
                        return false;
                    }
                    switch (brancketStack.pop()) {
                        case '(': {
                            if (ch != ')')
                                return false;
                            break;
                        }
                        case '[': {
                            if (ch != ']')
                                return false;
                            break;
                        }
                        case '}': {
                            if (ch != '}')
                                return false;
                            break;
                        }
                    }
                    break;
                }

                default:
                    break;
            }
        }
        return brancketStack.isEmpty();
    }

    private static boolean isOperator(char ch) {
        return ch == '+' || ch == '-' || ch == '*' || ch == '/';
    }

    private static int precedence(char op) {
        if (op == '+' || op == '-')
            return 1;
        if (op == '*' || op == '/')
            return 2;
        return 0;
    }

    public String transFix(String input) {
        StringBuilder output = new StringBuilder();
        Stack<Character> opsStack = new Stack<>();

        for (char ch : input.toCharArray()) {
            if (Character.isLetterOrDigit(ch)) {
                output.append(ch);
            } else if (isOperator(ch)) {
                while (!opsStack.isEmpty() && precedence(opsStack.peek()) >= precedence(ch)) {
                    output.append(opsStack.pop());
                }
                opsStack.push(ch);
            }
        }

        while (!opsStack.isEmpty()) {
            output.append(opsStack.pop());
        }

        return output.toString();
    }
}
