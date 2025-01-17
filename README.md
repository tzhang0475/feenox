---
title: FeenoX
subtitle: A free no-fee no-X uniX-like finite-element(ish) computational engineering tool
titleblock: |
 FeenoX: a free no-fee no-X uniX-like finite-element(ish) computational engineering tool
 =======================================================================================
lang: en-US
number-sections: true
toc: true
...


::: {.only-in-format .html .markdown}
> * [FeenoX annotated examples](https://www.seamplex.com/feenox/examples)
> * FeenoX Overview Presentation, August 2021
>   - [Recording (audio in Spanish, slides in English)](https://youtu.be/-RJ5qn7E9uE)
>   - [Slides in PDF](https://www.seamplex.com/feenox/doc/2021-feenox.pdf)
>   - [Markdown examples sources](https://github.com/gtheler/2021-presentation)
> * FeenoX for nuclear-related problems Presentation, December 2021
>   - [Recording with CCs](https://youtu.be/e8kFmFOsbPk)
>   - [Slides in PDF](https://www.seamplex.com/feenox/doc/2021-brasil.pdf)
>   - [Markdown examples sources](https://github.com/gtheler/2021-brasil)
:::


# Why FeenoX?

The world is already full of finite-element programs and every day a grad student creates a new one from scratch.
So why adding FeenoX to the already-crowded space of FEA tools?

```{.include}
doc/why.md
```


On the other hand, FeenoX meets a fictitious-yet-plausible [Software Requirement Specifications](https://www.seamplex.com/feenox/doc/srs.html) that no other single tool (that I am aware of) meets completely. The FeenoX [Software Design Specifications](https://www.seamplex.com/feenox/doc/sds.html) address each requirement of the SRS.
The two more important design-basis features are that FeenoX is...

 1. a cloud-first computational tool (not just cloud _friendly,_ but cloud **first**).
 2. both free (as in freedom) and open source. See [Licensing].


If by "Why FeenoX?" you mean "Why is FeenoX named that way?," read the [FAQs](doc/FAQ.md).


# What is FeenoX?

```{.include}
doc/introduction.md
```


# Download

```include
doc/download.md
```

## Git repository

```include
doc/git.md
```

See the [detailed compilation instructions](doc/compilation.md) for further details.

# Licensing

FeenoX is distributed under the terms of the [GNU General Public License](http://www.gnu.org/copyleft/gpl.html) version 3 or (at your option) any later version. The following text was borrowed from the [Gmsh documentation](http://gmsh.info/doc/texinfo/gmsh.html#Copying-conditions). Replacing “Gmsh” with “FeenoX” gives:

> ```include
> doc/freesw.md
> ```

```include
doc/licensing.md
```


# Further information

Home page: <https://www.seamplex.com/feenox>  
Repository: <https://github.com/seamplex/feenox>  
Bug reporting: <https://github.com/seamplex/feenox/issues>  
Discussions: <https://github.com/seamplex/feenox/discussions>  
Follow us: [YouTube](https://www.youtube.com/channel/UCC6SzVLxO8h6j5rLlfCQPhA)
           [LinkedIn](https://www.linkedin.com/company/seamplex/)
           [Github](https://github.com/seamplex)

---------------------------

FeenoX is copyright ©2009-2022 [Seamplex](https://www.seamplex.com)  
FeenoX is licensed under [GNU GPL version 3](http://www.gnu.org/copyleft/gpl.html) or (at your option) any later version.  
FeenoX is free software: you are free to change and redistribute it.  
There is **NO WARRANTY**, to the extent permitted by law.  
See the [copying conditions](COPYING).  
