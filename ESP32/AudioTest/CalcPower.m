function [Power] = CalcPower(input)

TotalEnergy = 0;
for n = 1:length(input)
    Energy = double((input(n,1)+input(n,2)))^2;
    TotalEnergy = TotalEnergy + Energy;
end

Power = TotalEnergy/length(input);

end

