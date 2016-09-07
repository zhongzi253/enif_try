-module(prime).  
-export([load/0, findPrime/1]).  

load() ->  
  erlang:load_nif("./cprime", 0).  

findPrime(N) ->  
  io:format("this function is not defined!~n").

create_tuple(Atom, Integar1, Intergar2) ->
  io:format("create_tuple is not defined!~n").


print_nif() ->
  Tuple1 = create_tuple(create, 12, 14),
  Tuple2 = create_tuple(second, 1, 4),
  io:format("Tuple1 = ~p~n Tuple2 = ~p~n", [Tuple1, Tuple2]).




