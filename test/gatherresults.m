function DB=gatherresults(dbpath,ext)
%create DB structure from all output files of stfeatures program in a
%specific directory
curpath=pwd;
cd(dbpath);
DB.files = dir(['*', ext]);
n = length(DB.files);
DB.orig=cell(n,1);; % a copy of the file
DB.Jets=cell(n,1);  %JET features #ips * 34
DB.pts=cell(n,1);   % ip coordinates [x y t]
DB.harris=cell(n,1); % hariss function value


for i = 1:n

try
    fname= getfield(DB.files,{i},'name');
    T = dlmread(fname);
    T=T(:,1:38);
    DB.orig{i}= T;
    DB.pts{i}= T(:,1:3);
    DB.harris{i} = T(:,4);
    DB.Jets{i} = T(:,5:38);
    
catch
   warning(['can cannot read file  ' fname]);
end
    
end
cd(curpath);