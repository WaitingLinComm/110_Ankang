// 110 Ankang OQPSK demod C++
// Execute instruction: 
// (1) g++ -o d main2.cpp -lfftw3 -lm
// (2) ./d
//
// NOTE:
// The code adopts fft/ifft implementation in FFTW library to perform the convolution of two complex vectors.
// Recall: conv(x,y) = ifft(fft(x).*fft(y)) where x and y are two complex vectors.
// To #include <fftw3.h>, one should follow instruction in https://www.fftw.org to install FFTW library.

main1.cpp 
    -Respectively process pilot & pilot + data in each burst.
     By doing so, we can have demod pilot symbols in each burst to calculate SNR & SER, and no need for extracting demod pilot symbols from the cascaded demd pilot + data symbols as in main2.cpp.
     However, we have to process pilot symbols two times which increases our processing time, 
     so in main2.cpp, we would only process pilot + data to speed up. But, we have to do extra step to extract demod pilot symbols from the cascaded demd pilot + data symbols.
    -When setting CFOPHOTO_estimator [MODE 1] and CFOPHOTO_estimator [MODE A] in main2.cpp, the demod process is reduced to main1.cpp.

main2.cpp 
    -Although analysis symbol in each burst is actually different, we first set the same number of analysis symbol in each burst(ex: K = 2000 means we get 2*K+1 = 4001 data per burst to do CFO, PHO, TO estimation
    -use find_high_pilot.m to find head position of the burst(i.e. the pilot position)
    -Cascade all the pilot + data in each burst and then go through DecisionProcess and aftend process. 
     We need to have received pilot symbols in each burst to calculate SNR and SER.  
     Due to the fact that number of processing data in each burst, K, is set in the previous iteration, we can extract
     received pilot symbols from pilot + data symbols.
    -With SNR estimation and Error Rate Calculation
	
    Speed up method
	- Set resample window form 6 to 1
	- Before implement coarseTO_estimator, 
	  we found that we can downsample  signal(:) and pilot(:) by 4 before calculating the above convolution. 
	  And then multiply  the position we find  by 4 would become the coarse pilot position.
	  Although there are some errors in the found coarse pilot position, we can correct the errors by the later CFOPHOTO_estimator
	- coarseTO_estimator 
		[MODE 1][original] only demod the first K symbols in each burst
                           use coarseTO_estimator.h
                                - sol1: use complex convolution
                                - sol2: use overlap add method  
		[MODE 2][faster] only demod the first K symbols in each burst    
                            use coarseTO_estimator_v2.h 
                                - speed up version of coarseTO_estimator.h.
                                - use complex_convolution_with_norm.h (merge complex_convolution.h with y1_norm2.h)
        [MODE 3][fastest] only demod the first K symbols in each burst
                Before implement coarseTO_estimator, we found that we can downsample  signal(:) and pilot(:) by 5 before calculating the above convolution. 
	            And then multiply  the position we find  by 5 would become the coarse pilot position.
	            Although there are some errors in the found coarse pilot position, we can correct the errors by the later CFOPHOTO_estimator.
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        [MODE 4][original] able to find the start & end position in each burst by using find_burst_interval.h and demod them 
                            use coarseTO_estimator_v3.h
                                - use complex_convolution_with_norm.h (merge complex_convolution.h with y1_norm2.h)
                                - use find_burst_interval.h to find the start position(pilot position) and the end position of each burst
        
        ///// NOTE: the end position of burst cannot be found correctly in __MODE_5__
        ///// NOTE: MODE 5 需搭配 [MODE D] 錯誤率才較低(若搭配 MODE A 錯誤率會高) 
        [MODE 5][faster] able to find the start & end position in each burst by using find_burst_interval.h and demod them  
                            use coarseTO_estimator_v3.h
                                - use complex_convolution_with_norm.h (merge complex_convolution.h with y1_norm2.h)
                                - use find_burst_interval.h to find the start position(pilot position) and the end position of each burst
                                Before implement coarseTO_estimator, we found that we can downsample  signal(:) and pilot(:) by 5 before calculating the above convolution. 
                                And then multiply  the position we find  by 5 would become the coarse pilot position.
                                Although there are some errors in the found coarse pilot position, we can correct the errors by the later CFOPHOTO_estimator.
        [MODE 6][faster] able to find the start & end position in each burst by using find_burst_interval.h and demod them  
                                - use complex_convolution_with_norm.h (merge complex_convolution.h with y1_norm2.h)
                                - use find_burst_interval.h to find the start position(pilot position) and the end position of each burst       
                                use K = 1000 data in each burst to do CFOPHOTO_estimator and compensate all the data in each burst.

        // MODE setting:
        [MODE 1], [MODE 2], [MODE 3]: only demod the first K symbols in each burst
        [MODE 4], [MODE 5], [MODE 6]: able to find the start & end position in each burst by using find_burst_interval.h and demod them
    
    - CFOPHOTO_estimator 
        [MODE A][original] CFOPHOTO_estimator:    original version (error pilot: 14/0.8s data)
        [MODE B][faster]   CFOPHOTO_estimator_v2: speed up version [reduce the needed while loop and for loop. Discriminant is also added to break the loop earlier] (error pilot: 101/0.8s data)
        [MODE C][fastest]  CFOPHOTO_estimator_v3: speed up version [only do one while loop] (error pilot: 78/0.8s data)
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        [MODE D]           CFOPHOTO_estimator_v4: used with [MODE 4] or [MODE 5] or [MODE 6]

    // MODE setting Combination:
    [MODE A] can be used with [MODE 1] or [MODE 2] or [MODE 3] or [MODE 4] or [MODE 5] or [MODE 6]   
    [MODE B] can be used with [MODE 1] or [MODE 2] or [MODE 3]
    [MODE C] can be used with [MODE 1] or [MODE 2] or [MODE 3]
    [MODE D] can be used with [MODE 4] or [MODE 5] or [MODE 6] 

    - When setting CFOPHOTO_estimator [MODE 1] and CFOPHOTO_estimator [MODE A], we have the most accurate OQPSK demod result, but it takes longest computation time.
    - When setting CFOPHOTO_estimator [MODE 3] and CFOPHOTO_estimator [MODE C], the OQPSK demod result is not that accurate, but it takes least computation time.

