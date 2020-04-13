function [Serialized] = SerializeToFile(input)

Serialized = zeros(1, 2*length(input), 'int16');
row = 1;
for n = 1:2*length(input)
    if(mod(n, 2) == 1)
        Serialized(n) = input(row,1);
    else
        Serialized(n) = input(row,2);
        row = row + 1;
    end
end
Serialized = Serialized';

fid = fopen('Input/InputFile.data', 'w');
if fid == -1
    error('Could not open input file')
end

fwrite(fid, Serialized, 'int16');
fclose(fid);

end

