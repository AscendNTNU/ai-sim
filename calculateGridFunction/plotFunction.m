function plotFunction(polynomialOrder, coeff)
    figure
    x=linspace(1, 20);
    y=linspace(1, 20);
    [X,Y]=meshgrid(x,y);
    Z = calculateReward(X,Y,polynomialOrder,coeff);
    surf(X,Y,Z);
    shading interp
    axis tight
    zlim([-1000,2000])
    xlabel('x')
    ylabel('y')
end