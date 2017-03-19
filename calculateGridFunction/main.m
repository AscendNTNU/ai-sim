format shortG
clc
clear all

% Grid values constructed by value iteration
values = createGrid();
num_points = size(values,1)*size(values, 2);

% Order of polynomial to fit to grid values
polynomialOrder = 8; % Change this to desired order (max 7)
numberOfTerms = [3, 6, 10, 15, 21, 28, 36, 45];

row = 1;
F = zeros(num_points,1);
A = zeros(num_points,numberOfTerms(end));
for X=1:20
    for Y=1:20
        % Ordering grid values
        F(row) = values(Y,X);
        
        % Construction design matrix
        A(row,:) = [...
            1, X, Y, ...
            X^2, X*Y, Y^2, ...
            X^3, X^2*Y, X*Y^2, Y^3, ...
            X^4, X^3*Y, X^2*Y^2, X*Y^3, Y^4, ...
            X^5, X^4*Y, X^3*Y^2, X^2*Y^3, X*Y^4, Y^5, ...
            X^6, X^5*Y, X^4*Y^2, X^3*Y^3, X^2*Y^4, X*Y^5, Y^6, ...
            X^7, X^6*Y, X^5*Y^2, X^4*Y^3, X^3*Y^4, X^2*Y^5, X*Y^6, Y^7,...
            X^8, X^7*Y, X^6*Y^2, X^5*Y^3, X^4*Y^4, X^3*Y^5, X^2*Y^6, X*Y^7, Y^8,...
        ];
        
        row = row + 1;
    end
end

% Construction weight matrix
P = diag(zeros(num_points,1)+1);

% Removing terms of higher order than specified
numTerms = numberOfTerms(polynomialOrder);
A(:,numTerms+1:size(A,2)) = [];

% Estimating coefficients by LSM
coeff = (A'*P*A)^(-1)*A'*P*F;

% Plot function
plotFunction(polynomialOrder, coeff)

% Plot difference between function values and grid values
plotDifference(values,polynomialOrder,coeff);

% Save function as text in equation.txt
formatEquationForCPP(coeff);
