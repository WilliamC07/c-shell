The only quotes you may have are double quotes. Having single quotes will break it. This is not a bug, it is done on purpose to not destroy my code and make ths over complicated.
[Source I used for differentiating between double and single quotes that caused me to make this decision](https://stackoverflow.com/questions/6697753/difference-between-single-and-double-quotes-in-bash).

In bash and zsh, starting a command with a space means it will not be stored in history. In this shell, this feature does not exist. 

The only escape character supported by strings is:
    \" -- Escapes quotes
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