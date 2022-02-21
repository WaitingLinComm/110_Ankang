110 Ankang Project

This is the matlab code for demodulating OQPSK signal
- data file: burst2_sr6.25_dtCplx_st8t
- Execute main_Ankang_OQPSK.m

main_Ankang_OQPSKv1.m
	-test with a specific number of data, ex: 1e6 samples.
	-test with one of the burst.
	-Although analysis symbol in each burst is actually different, we first set the same number of analysis symbol in each burst(ex: K = 2000 means we get 2*K+1 = 4001 data per burst to do CFO, PHO, TO estimation
	-use find_high_pilot.m to find head position of the burst(i.e. the pilot position)
	-PHO_aftendRx_Optimization can be tested in main_Ankang_OQPSKv1.m. It is the extra demodulation method we have tried.
	-go see main_Ankang_OQPSK_system_block

main_Ankang_OQPSKv2.m [最完整版]
	-test with a specific number of data, ex: 1e6 samples.
	-cascade all the data in each burst and then go through DecisionProcess and aftend process
	
	MODE 1: let analysis symbol in each burst be different[correspond to the given Ankang OQPSK signal]
			use get_databurst_v2.m	
			use coarseTO_estimator_v2.m which adopts find_burst_interval.m to find the head position(i.e. the pilot position) and the end position of the burst
	MODE 2: let analysis symbol in each burst be the same[simplified version]
	-go see main_Ankang_OQPSK_system_block

main_Ankang_OQPSKv2_with_timing.m [最完整版+timing]
	-calculate time-consuming of each process in main_Ankang_OQPSKv2_2.m

main_Ankang_OQPSKv3.m
	-test with a specific number of data, ex: 1e6 samples.
	-go through DecisionProcess and aftend process directly after getting data in each burst
	MODE 1: let analysis symbol in each burst be different[correspond to the given Ankang OQPSK signal]
		use get_databurst_v2.m	
		use coarseTO_estimator_v2.m which adopts find_burst_interval.m to find the head position(i.e. the pilot position) and the end position of the burst
	MODE 2: let analysis symbol in each burst be the same[simplified version]
	-go see main_Ankang_OQPSK_system_block

main_Ankang_OQPSKv4_with_timing.m [加速版+timing]	
	-test with a specific number of data, ex: 1e6 samples.
	-cascade all the data in each burst and then go through DecisionProcess and aftend process
		MODE 1: let analysis symbol in each burst be different[correspond to the characteristic of given Ankang OQPSK signal]
				use get_databurst_v2.m	
				use coarseTO_estimator_v2.m which adopts find_burst_interval.m to find the head position(i.e. the pilot position) and the end position of the burst
		MODE 2: let analysis symbol in each burst be the same[simplified version. K is needed to be set in the code]
	-To calculate SNR and SER through received pilot, received pilots are extracted through the cascaded data.[in the case when MODE 2 is used, i.e. K is set] 

	Speed up method
	- Set resample window form 6 to 1
	- Before implement coarseTO_estimator, 
	  we found that we can downsample  signal(:) and pilot(:) by 4 before calculating the above convolution. 
	  And then multiply  the position we find  by 4 would become the coarse pilot position.
	  Although there are some errors in the found coarse pilot position, we can correct the errors by the later CFOPHOTO_estimator
	- CFOPHOTO_estimator
		[Version 1] CFOPHOTO_estimator:    original version
		[Version 2] CFOPHOTO_estimator_v2: speed up version[reduce the needed while loop and for loop. Discriminant is also added to break the loop earlier.]
		[Version 3] CFOPHOTO_estimator_v3: speed up version[only do one while loop]
		
Other code:
coarseTO_estimator_v2_with_overlapadd.m
	-test convolution with overlap add method
		overlap add method v1: written by Wei-Ting Lin
		overlap add method v2: Ref: https://alan23273850.gitbook.io/signals-and-systems/di-wu-zhang-li-san-fu-li-ye-zhuan-huan-dft/di-liu-jie-dft-de-ying-yong-san-ji-suan-da-gui-mo-shi-yong-de-xian-xing-zhe-ji


	
	
	
	
	
	

