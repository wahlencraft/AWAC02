\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{awac02}[2022/08/11 The class for AWAC documents]

\LoadClass{article}

\usepackage[a4paper, total={6in, 8in}]{geometry}

\renewcommand{\maketitle}{%
    {
        \hspace{-15pt}\fontsize{50}{50}\fontfamily{phv}\fontseries{b}\selectfont
        AWAC02 \hfill \vspace{4pt}\newline
        \fontsize{20}{25}\fontfamily{phv}\fontseries{b}\fontshape{sl}\selectfont \@title
    }
    \clearpage
    \pagenumbering{arabic}
}

\renewcommand{\section}{\@startsection{section}
    % Level
    {1}
    % Indent
    {0pt}
    % Beforeskip
    {-3.5ex \@plus -1ex \@minus -.2ex}
    % Afterskip
    {2.3ex \@plus.2ex}
    % Style
    {\newpage\fontsize{15}{20}\fontfamily{phv}\fontseries{b}\slshape}
}

\pagenumbering{gobble} % Turn off page-numbering for the first page
