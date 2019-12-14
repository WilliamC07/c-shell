The only quotes you may have are double quotes. Having single quotes will break it. This is not a bug, it is done on purpose to not destroy my code and make ths over complicated.
[Source I used for differentiating between double and single quotes that caused me to make this decision](https://stackoverflow.com/questions/6697753/difference-between-single-and-double-quotes-in-bash).

In bash and zsh, starting a command with a space means it will not be stored in history. In this shell, this feature does not exist. 

The only escape character supported by strings is:
    \" -- Escapes quotes
    \n -- New line character
All others will just show the "\" and following character (ex. "\t" --> "\t")

Bugs:
- ```cd``` only takes in two arguments: "cd" and the following argument. It will silently ignore the rest  
- ```exit``` ignores the subsequent arguments

temp: some great edge caseing
echo "\\" --> \\  
echo \t" --> \t"  
echo "\t --> "\t  
echo "\\t" --> \\t  
echo \""" --> \"""  

Having escaped characters that is not in an double quoted argument will not be escaped.  
Ex: echo \t"asd" --> \t"asd"  

Entered quotes must be balanced.  
doing echo \" will print "\"" and not "  

I did not support chaining pips  

Special operation supports:  
">", "<", and "|"  
We do not support:  
Anything not listed above like ">>" or "<<"

Cannot redirect thing into cd.  
```cd < file_with_directory``` does not work

redirecting a file to this shell might have missing commands printing out (still runs, you just don't see the program on stdout, aka your terminal).
