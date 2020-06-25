# Spell Checker

The spellchecker is intended to correct typos if set up. We had tried a fuzzy mode for queries earlier, which 
resulted in weird false-positives as well as a delay in query processing. This spell-checker comes into action
only if a user-typed full word is not there ( or nominally there ).

You can test the spellchecker by using the `zpds_spell` program , give it a file with one word per line.

If want to enable spellcheck , add a file for training at the location `jinpath` as EN.txt . The jinpath can be set in config and 
can be overridden at command line. Note this is needed only once so subsequent restarts will not need this.

