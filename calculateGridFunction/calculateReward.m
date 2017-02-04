function Z = calculateReward(X,Y,polynomialOrder,coeff)

    Z = coeff(1) + coeff(2).*X + coeff(3).*Y;
    if polynomialOrder > 1
        Z = Z + coeff(4).*X.^2 + coeff(5).*X.*Y + coeff(6).*Y.^2;
    end
    if polynomialOrder > 2
        Z = Z + coeff(7).*X.^3 + coeff(8).*X.^2.*Y + ...
            coeff(9).*X.*Y.^2 + coeff(10).*Y.^3;
    end
    if polynomialOrder > 3
        Z = Z + coeff(11).*X.^4 + coeff(12).*X.^3.*Y + coeff(13).*X.^2.*Y.^2 + ...
            coeff(14).*X.*Y.^3 + coeff(15).*Y.^4;
    end
    if polynomialOrder > 4
        Z = Z + coeff(16).*X.^5 + coeff(17).*X.^4.*Y + coeff(18).*X.^3.*Y.^2 + ...
            coeff(19).*X.^2.*Y.^3 + coeff(20).*X.*Y.^4 + coeff(21).*Y.^5;
    end
    if polynomialOrder > 5
        Z = Z + coeff(22).*X.^6 + coeff(23).*X.^5.*Y + coeff(24).*X.^4.*Y.^2 + ...
            coeff(25).*X.^3.*Y.^3 + coeff(26).*X.^2.*Y.^4 + coeff(27).*X.*Y.^5 + ...
            coeff(28).*Y.^6;
    end
    if polynomialOrder > 6
        Z = Z + coeff(29).*X.^7 + coeff(30).*X.^6.*Y + coeff(31).*X.^5.*Y.^2 + ...
            coeff(32).*X.^4.*Y.^3 + coeff(33).*X.^3.*Y.^4 + coeff(34).*X.^2.*Y.^5 + ...
            coeff(35).*X.*Y.^6 + coeff(36).*Y.^7;
    end

end