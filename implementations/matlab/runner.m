function runner(page_size,num_pages,num_execs,lut_file)
% Example: runner(65536,128,1);
if mod(page_size, 8) ~= 0
    error('Unsupported page size of %u, please choose a page size that is a multiple of 8\n',page_size);
end

s = RandStream('mcg16807','Seed',49734321);
% RandStream.setDefaultStream(s);
RandStream.setGlobalStream(s);
seed = 10000;

h_num = randi(seed, num_pages, page_size); %rand_crc

expected_crc = 2231263667;
% TODO: Add result checking either by using a random
%       number generator similar to C and JavaScript
%       or by generating the input data outside
%check_result = and(page_size == 65536, num_pages == 128);
check_result = false;

fileID = fopen(lut_file, 'r');
crc32Lookup = reshape(fscanf(fileID,'%x'),256,8)';
fclose(fileID);

tic
for j = 1:num_execs
    final_crc = crc_ostrich(page_size,num_pages,h_num,crc32Lookup);
	if check_result
	    if final_crc ~= expected_crc
	        error('Invalid crc check, received %u while expecting %u\n', final_crc, expected_crc);
	    end
	end
end
elapsedTime = toc;



if not(and(page_size == 65536, num_pages == 128))
    warning('WARNING: no self-checking step for page_size %u and num_pages %u\n', page_size, num_pages);
end

fprintf('{ \"status\": %d, \"options\": \"-n %d -s %d -r %d\", \"time\": %f }\n', 1, num_pages, page_size, num_execs, elapsedTime);

end