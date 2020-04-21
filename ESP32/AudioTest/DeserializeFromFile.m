function [raw, Deserialized] = DeserializeFromFile(length_origin)

% Reading data from outputfile
fid = fopen('Output/OutputFile.data');
if fid == -1
    error('Could not open output file')
end

raw = fread(fid, length_origin, 'int16=>int16', 'ieee-be');

fclose(fid);

% Deserialized into two channels

Deserialized = zeros(length_origin/2, 2, 'int16');

row = 1;
for n = 1:length(raw)
    if(mod(n, 2) == 1)
        Deserialized(row, 1) = raw(n);
    else
        Deserialized(row, 2) = raw(n);
        row = row + 1;
    end
end

end

