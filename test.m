clear all;

calculate_mean = false;
show_mean = false;
find_outliers = true;
calculate_mask = false;
calculate_weight = false;

n_images_mean = 250;
n_images_test = 4;

fixed = crop_image(imread('Images/ID0001.tif'));
%moving = crop_image(imread('Images/ID0002.tif'));
% imshow(crop_image(moving));

if calculate_mean
    sum_img = double(fixed);
    entries = randperm(5000, n_images_mean);
    for n = 1:size(entries, 2)
        movingReg = imgaussfilt3(load_and_align(fixed, entries(n)), 1);
        sum_img = sum_img + double(movingReg);
    end
    mean_img = uint8(sum_img / n_images_mean);

    save('mean_img.mat', 'mean_img');
else
    load('mean_img.mat');
end

if show_mean
    figure(1)
    imshow(mean_img);
end

if calculate_mask
	img = rgb2gray(mean_img);
    mask = ones(size(img));
    bw = activecontour(img, mask, 600);
    mask = uint8(ordfilt2(bw, 1, true(20)));
    mask(1155:1200, 710:840) = 0;
    save('mask.mat', 'mask');
else
    load('mask.mat');
end

if calculate_weight
    Gx = [1, 2, 1; 0, 0, 0; -1, -2, -1];
    Gy = Gx';
    mean_gray = rgb2gray(mean_img);
    Ix = abs(conv2(mean_gray, Gx, 'same'));
    Ix(Ix < 75) = 0;
    Ix = bwareaopen(imerode(Ix, strel('square',3)), 40);
    filtered = imgaussfilt(double(Ix), 2);
    image_weight = 1 - mat2gray(filtered);
    image_weight = image_weight .* double(mask);
    save('image_weight.mat', 'image_weight');
else
    load('image_weight.mat');
end

if find_outliers
    trouble = [3, 7, 25, 96, 114, 212, 265, 330, 374, 378, 411, 451, 632];
    for n = trouble %1:size(trouble,2)
        movingReg = load_and_align(mean_img, n, 0.5); %330 2560
        fprintf('Images/ID%04d.tif\n',n);
        diff = calculate_score(mean_img, movingReg, image_weight);
        diff(diff < 150) = 0; % 185
        outliers = bwmorph(uint8(diff) .* mask, 'shrink');

        if sum(outliers(:)) > 0
            disp(['Outlier ', int2str(n)]);
            save_image(movingReg, outliers, n);
        %else
        %    disp(['Not outlier ', int2str(n)]);
        end
    end
end

% 470:500 - 115 sec

% Outlier 212 - outlier (1)
% Outlier 217
% Outlier 230
% Outlier 251
% Outlier 262
% Outlier 308
% Outlier 330 - outlier (7)
% Outlier 353
% Outlier 361 - (outlier) (9)


% 330 - e
% 632 - t
% 1051
% (2375)
% 2612 - k
% 3100 - e
% 4491 - g

% 
% movingReg = load_and_align(mean_img, 1051, 1);
% %movingReg = movingReg(325:1080, 115:1700, :);
% %mean_img = mean_img(325:1080, 115:1700, :);
% 
% diff = calculate_score(mean_img, movingReg, image_weight);
% diff(diff < 185) = 0;
% outliers = bwmorph(uint8(diff) .* mask, 'shrink');
% disp(['Outlier pixels: ', int2str(sum(outliers(:)))]);
% 
% figure(2)
% imshow(movingReg);

function save_image(img, outliers, n)
    f = figure('visible', 'off'), imshow(img);
    hold on
    r = 40;
    [y, x] = find(outliers > 0, 1);
    th = 0:pi/50:2*pi;
    xunit = r * cos(th) + x;
    yunit = r * sin(th) + y;
    plot(xunit, yunit, 'r', 'LineWidth', 3);
    print(f, '-r80', '-dtiff', sprintf('outliers/ID%04d.tif',n));
end


function diff = calculate_score(fixed, moving, image_weight)
    diff = sqrt(sum((double(fixed) - double(moving)).^2, 3)) .* image_weight;
end

function new_image = load_and_align(fixed, n, scaleFactorDown)
    new_image = crop_image(imread(sprintf('Images/ID%04d.tif',n)));
    
    if scaleFactorDown == 1
        tformEstimate = imregcorr(new_image,fixed,'rigid');
    else    
        new_image_scaled = imresize(new_image, scaleFactorDown, 'bilinear');
        fixed_scaled = imresize(fixed, scaleFactorDown, 'bilinear');
        tformEstimate = imregcorr(new_image_scaled,fixed_scaled,'rigid');
        tformEstimate.T(3,1:2) = tformEstimate.T(3,1:2) .* 1/scaleFactorDown;
    end
    new_image = imwarp(new_image,tformEstimate,'OutputView',imref2d(size(fixed)));
end

function crop = crop_image(img)
    crop = img(140:1560, 150:1898, :);
end
