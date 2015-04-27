
% this is a simple demo to show how to use stfeatures program
% compute st features for all movie files in the current path
% and store the results in DB structure
% see also gatherresults


% path to the program
stpath='../bin/stfeatures'

avis = dir(['*', '.avi']);
for i = 1:length(avis);
    fname= getfield(DB.files,{i},'name');
    cmd=[stpath ' -f ' fname ' -o ' fname '.stf' ' -sigma 2 -tau  8 '];
    system(cmd);
end

DB=gatherresults('.','.stf');