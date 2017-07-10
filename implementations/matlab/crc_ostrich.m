function [final_crc] = crc_ostrich(page_size,num_pages,h_num,crc32Lookup)
% Core computation
crcs = zeros(1,num_pages);
num_words = page_size / 4;
for i = 1:num_pages
    %h_sta = (i - 1) * num_words + 1;
    %h_end = h_sta + num_words - 1;
    %crcs(i) = crc32_8bytes(h_num(h_sta:h_end), page_size, crc32Lookup);
    crcs(i) = crc32_8bytes(h_num(i,1:num_words), page_size, crc32Lookup);
end

final_crc = crc32_8bytes(crcs(1:num_pages), 4 * num_pages, crc32Lookup);
end