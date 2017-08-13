function runner(page_size,num_pages,num_execs,lut_file,input_directory)
% Example: runner(65536,128,1);
if mod(page_size, 8) ~= 0
    error('Unsupported page size of %u, please choose a page size that is a multiple of 8\n',page_size);
end

s = RandStream('mcg16807','Seed',49734321);
% RandStream.setDefaultStream(s);
RandStream.setGlobalStream(s);
seed = 10000;


expected_crc = 2231263667;

fileID = fopen(lut_file, 'r');
crc32Lookup = reshape(fscanf(fileID,'%x'),256,8)';
fclose(fileID);

file_path = fullfile(input_directory, strcat(num2str(page_size), '-', num2str(num_pages), '-data.csv'));
if exist(file_path) 
    fprintf('Loaded data from %s', file_path);
    num_words = page_size/4;
    h_num = reshape(uint32(load(file_path)), num_words, num_pages);
else
    fprintf('%s does not exist, generating data instead', file_path);
    h_num = randi(seed, num_words, num_pages); %rand_crc
end

tic
for j = 1:num_execs
    final_crc = crc_ostrich(page_size,num_pages,h_num,crc32Lookup);
end
elapsedTime = toc;

int32_crc = typecast(final_crc, 'int32');
final_crc = int32_crc(1);

fprintf('{ \"status\": %d, \"options\": \"-n %d -s %d -r %d\", \"time\": %f, \"output\": %d }\n', 1, num_pages, page_size, num_execs, elapsedTime, final_crc);

end

