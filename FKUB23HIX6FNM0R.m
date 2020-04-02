function [output] = readValue(s,command)
% Serial send read request to Arduino
fprintf(s,command);  

% Read value returned via Serial communication 
output = fscanf(s,'%f %f %f %f %f');
output = output';

end
