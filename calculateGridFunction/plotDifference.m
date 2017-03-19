function plotDifference(grid,polynomialOrder,coeff)
    functionValue = zeros(20, 20);

    for i=1:20
        for j=1:20
            functionValue(j,i) = calculateReward(i,j,polynomialOrder,coeff);
        end
    end

    figure
    clear title xlabel ylabel
    surf(grid)
    xlabel('x')
    ylabel('y')
    zlim([-1000,2000])

    figure
    clear title xlabel ylabel
    surf(functionValue)
    xlabel('x')
    ylabel('y')
    zlim([-1000,2000])

    figure
    clear title xlabel ylabel
    surf(grid-functionValue)
    xlabel('x')
    ylabel('y')
    zlim([-1000,1000])
    
%     filename = sprintf('error/%d_function_grid_error.png',polynomialOrder);
%     saveas(gcf,filename)
end