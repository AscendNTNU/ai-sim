function valueGrid = createGrid()
    m = zeros(22,22);
    m(:,1) = -1000;
    m(:,22)= -1000;
    m(1,:) = -1000;
    m(22,:)=  2000;

    for k=1:10000
        for i=2:21
            for j=2:21
                m(i,j) = (m(i-1,j)+m(i+1,j)+m(i,j-1)+m(i,j+1))/4;
            end
        end

        for i=21:2
            for j=21:2
                m(i,j) = (m(i-1,j)+m(i+1,j)+m(i,j-1)+m(i,j+1))/4;
            end
        end

        for i=21:2
            for j=2:21
                m(i,j) = (m(i-1,j)+m(i+1,j)+m(i,j-1)+m(i,j+1))/4;
            end
        end

        for i=2:21
            for j=21:2
                m(i,j) = (m(i-1,j)+m(i+1,j)+m(i,j-1)+m(i,j+1))/4;
            end
        end
    end
    valueGrid = m(2:21,2:21);
end