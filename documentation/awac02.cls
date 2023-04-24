\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{awac02}[2023/04/24 The class for AWAC documents]

\newcommand{\version}{v0.0}

\LoadClass{article}

\usepackage[a4paper, total={6.5in, 8in}]{geometry}

\renewcommand{\maketitle}{%
    {
        \hspace{-15pt}\fontsize{50}{50}\fontfamily{phv}\fontseries{b}\selectfont
        AWAC02 \hfill \vspace{4pt}\newline
        \fontsize{20}{25}\fontfamily{phv}\fontseries{b}\fontshape{sl}\selectfont \@title
        \vfill\hfill
        \fontsize{20}{25}\fontfamily{phv}\fontseries{b}\fontshape{n}\selectfont \version
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

\renewcommand{\subsection}{\@startsection{subsection}
    % Level
    {2}
    % Indent
    {0pt}
    % Beforeskip
    {-2.5ex \@plus -1ex \@minus -.2ex}
    % Afterskip
    {1.3ex \@plus.2ex}
    % Style
    {\fontsize{13}{15}\fontfamily{phv}\fontseries{b}\selectfont}
}

\pagenumbering{gobble} % Turn off page-numbering for the first page

% Header
\usepackage{fancyhdr}
\setlength{\headheight}{30pt}
\pagestyle{fancy}
\fancyhf{}
\fancyhead[EOL]{\fontsize{20}{30}\fontfamily{phv}\fontseries{b}\selectfont AWAC02
                \fontsize{10}{20}\fontfamily{phv}\fontseries{b}\selectfont \version
} 
\fancyhead[EOR]{\fontsize{12}{17}\fontfamily{phv}\fontseries{b}\fontshape{sl}\selectfont \@title}
\fancyfoot[EOC]{\thepage}
\thispagestyle{empty}
