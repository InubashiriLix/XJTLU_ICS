(* let () = print_endline "Hello, World!" *)
 
(* List *)
(* 1. [] (nil) *)

(* 2. prepend e1 to e2, where e2 is an list *)
(* we can use cons "::" to implement this *)
(* 1 :: 2 :: 3 :: [] => [1; 2; 3] (int list) *)
(* 1 is the head, the rest are tails *)

(* 3. construct by written in normal way *)
(* [1; 2; 3];; *)
(* the square bracket syntax is conviennt but unnessary *)
(* it is recommanded to use method 2, which is an syntactic sugar *)

(* if ei evaluates to vi for all i in 1..n, then [e1; ...; en] evaluates to [v1; ...; vn] *)
(* and all the elements in the list should have the same type (staic semantics) *)

(* tell if one list is empty or not *)
(* let empty lst =  *)
(*     match lst with *)
(*     | [] -> true *)
(*     | h :: t -> false *)
(* ;; *)

let empty lst = lst = [];;

let lst1 = [1; 2; 3];;
let lst2 = [];;
let lst3 = [4; 5];;

let rec sum lst = 
    match lst with
    | [] -> 0
    | h :: t -> h + sum t
;;

let rec length lst = 
    match lst with 
    | [] -> 0
    | h :: t -> 1 + length t 
;;

let rec append lst1 lst2 = 
    match lst1 with 
    | [] -> lst2
    | h :: t -> h :: append t lst2
;;

let print_int_list lst =
  let rec aux = function
    | [] -> ""
    | [x] -> string_of_int x
    | x :: xs -> string_of_int x ^ "; " ^ aux xs
  in
  print_endline ("[" ^ aux lst ^ "]")
;;

let inc_first lst = 
    match lst with 
    | [] -> []
    | h::t -> (h + 1) :: t
;;

let () = 
    print_endline (string_of_bool (empty lst1));
    print_endline (string_of_bool (empty lst2));
    print_endline (string_of_int (sum lst1));
    print_endline (string_of_int (length lst1))
    let () = print_int_list(append lst1 lst3)
    let () = print_int_list(lst1 @ lst3)
    let () = print_int_list(inc_first lst1)
