== Introduction ==
it provide silver_decoder.so from silver_decoder,c in which new decoder functions is registered as nif. silver_decoder.erl is the bridge of the nif function and Erlang process. 

== Change in S1AP-PDU-Contents ==
15673 dec_os_Value138(Bytes, 213) ->
15674 %{Term_, _} = 'S1AP-IEs':dec_InformationOnRecommendedCellsAndENBsForPaging(Bytes),
15675 Term = silver_decoder:dec_InformationOnRecommendedCellsAndENBsForPaging(Bytes),
15676 Term.

== How to do function test ==
after uploading silver_decoder.so and silver_decoder.beam to right filecache, launch erl, do follows:
1. nl(silver_decoder)
2. silver_decoder:load()  //pay attention to the path of .so, you may need change it
3. silver_decoder:test1() or test2()

== How to do ST test ==
after uploading silver_decoder.so, silver_decoder.beam and S1AP-PDU-Contents.beam to right filecache, launch erl, do follows:
1. nl(silver_decoder)
2. nl('S1AP-PDU-Contents')
3. run traffic...  
!!!unfortunatedly, crash might happen in ST node. Now, no time to do further investigation

== How to compile silver_decoder.c ==
1. for GTT, 
gcc -g -fPIC -shared -o silver_decoder.so silver_decoder.c -I /vobs/gsn_rel_otp/built/c_linux_gpb_lws/usr/include/
2. for real Node
/vobs/gsn_rel_windriver/wrlinux/sysroots/gpb_gep3-glibc_cgl/tc_wrapping/i586-wrs-linux-gnu-x86_64-glibc_cgl-gcc -g -fPIC -shared -o silver_decoder.so silver_decoder.c -I /vobs/gsn_rel_otp/built/c_linux_gpb_lws/usr/include/

== Leftover ==
ST test
Erlang VM crash when run real traffic

