-module(silver_decoder).
-export([
        load_local/0,
        load/0,
        dec_InformationOnRecommendedCellsAndENBsForPaging/1,
        test1/0,
	test2/0,
        test_load/0
    ]).


load_local() ->
    %% the module call erlang:load_nif should be consistent with the name of the lib
    %%erlang:load_nif("/tmp/DPE_ROOT/SoftwareConfigurations/vob_tecsas/LoadUnits/FileCache/silver_decoder", 0).
    erlang:load_nif("/tmp/DPE_ROOT/SoftwareConfigurations/CurrentSC/LoadUnits/FileCache/silver_decoder", 0).

load() ->
    Nodes = [erlang:node() | erlang:nodes()],
    rpc:multicall(
        Nodes,
        silver_decoder,
        load_local,
        []).
test_load() ->
    erlang:load_nif("./silver_decoder", 0).
dec_InformationOnRecommendedCellsAndENBsForPaging(_Bytes) ->
    io:format("Not defined! ~n").

test1() -> %%1 ndb, 1 cell
    Bytes = <<1,0,214,64,10,64,33,67,101,0,0,16,16,0,30,0,214,64,10,64,33,67,101,0,0,16,16,0,40,0,0,215,64,8,0,33,67,101,0,0,0,16>>,
    io:format("Bytes = [~p]~n", [Bytes]),
    Res = dec_InformationOnRecommendedCellsAndENBsForPaging(Bytes),
    io:format("Res = [~p]~n", [Res]).
test2() -> %%16 ndb, 16 cell
    Bytes = <<15,0,214,64,10,64,33,67,101,0,0,16,16,0,31,0,214,64,10,64,33,67,101,0,0,
    16,16,0,32,0,214,64,10,64,33,67,101,0,0,16,16,0,33,0,214,64,10,64,33,67,
    101,0,0,16,16,0,34,0,214,64,10,64,33,67,101,0,0,16,16,0,35,0,214,64,10,
    64,33,67,101,0,0,16,16,0,36,0,214,64,10,64,33,67,101,0,0,16,16,0,37,0,
    214,64,10,64,33,67,101,0,0,16,16,0,38,0,214,64,10,64,33,67,101,0,0,16,16,
    0,39,0,214,64,10,64,33,67,101,0,0,16,16,0,50,0,214,64,10,64,33,67,101,0,
    0,16,16,0,60,0,214,64,10,64,33,67,101,0,0,16,16,0,70,0,214,64,10,64,33,
    67,101,0,0,16,16,0,80,0,214,64,10,64,33,67,101,0,0,16,16,0,90,0,214,64,
    10,64,33,67,101,0,0,16,16,0,30,0,214,64,10,64,33,67,101,0,0,16,16,0,40,
    60,0,215,64,8,0,33,67,101,0,0,0,16,0,215,64,8,0,33,67,101,0,0,0,16,0,215,
    64,8,0,33,67,101,0,0,0,16,0,215,64,8,0,33,67,101,0,0,0,16,0,215,64,8,0,
    33,67,101,0,0,0,16,0,215,64,8,0,33,67,101,0,0,0,16,0,215,64,8,0,33,67,
    101,0,0,0,16,0,215,64,8,0,33,67,101,0,0,0,16,0,215,64,8,0,33,67,101,0,0,
    0,16,0,215,64,8,0,33,67,101,0,0,0,16,0,215,64,8,0,33,67,101,0,0,0,16,0,
    215,64,8,0,33,67,101,0,0,0,16,0,215,64,8,0,33,67,101,0,0,0,16,0,215,64,8,
    0,33,67,101,0,0,0,16,0,215,64,8,0,33,67,101,0,0,0,16,0,215,64,8,0,33,67,
    101,0,0,0,16>>,
    io:format("Bytes = [~p]~n", [Bytes]),
    Res = dec_InformationOnRecommendedCellsAndENBsForPaging(Bytes),
    io:format("Res = [~p]~n", [Res]).

