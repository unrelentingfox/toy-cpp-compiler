%Script to add tree node actions to each production rule in bison grammar
%Input file should only contain rules between %%. Assumes there are no 
%syntax errors in grammer (compiles). 
%
%other notes: comments are ignored and existing actions are removed

fileID = fopen('bison.y', 'r');

%Read file of grammar rules (file should only contain stuff between %%);
A = fscanf(fileID, '%c');
newtext = regexprep(A,'([/][*]([^*]|[*]+[^/*])*[*]+[/])|[^''][{][^''{}]*[}][^'']', ' ');
fclose(fileID); 

%breakup text by each rule (left hand side -> ;)
token_rules = regexp(newtext, '([a-zA-Z0-9_]+[:](?:[^;]|'';'')*(?<!'')[;](?!''))', 'tokens');


%Loop to store left hand and right hand side of each rule and each
%seperaretly. Right hand side is tokenized by each terminal, nonterminal,
%"|", and ";".
lefthand = struct(); 
rules = cell(numel(token_rules), 1);
for i = 1:numel(token_rules) 
    rule = struct('lefthand', '', 'righthand', []); 
    right = struct;
    currRule = char(token_rules{i});
    rule.lefthand = char(regexp(currRule, '^[a-zA-Z0-9_]+[:]', 'match'));
    currRule = currRule(size(rule.lefthand, 2)+1:end);
    currProductions = regexp(currRule, ... 
        '((?<!'')[^ \f\n\r\t\v|;'']+(?!''))|(;)|((?<!'')[|])(?!'')|(''[^ \f\n\r\t\v]'')', ...
        'tokens');
    rule.righthand = currProductions;
    rules{i} = rule;
end


%Loop to write out "formatted" rules with each production rule 
%having an action with a call to a tree node creation funciton
%Note: Heavy use of string concatenation -> not optimized
fileID = fopen('Path_to_output_file', 'w');
for i = 1:numel(rules)
    curr_rule = rules{i};
    fprintf(fileID, '%s\n', curr_rule.lefthand);
    curr_right = curr_rule.righthand;
    j = 1;
    tabstr = '     ';
    while j < numel(curr_right)
       treestr = ['          { $$ = makeTreeNode(', ... %edit as needed 
                   curr_rule.lefthand(1:end-1),', '];
       numKids = 0;
       while isempty(regexp(char(curr_right{j}), '^[|;]$', 'match'))
            numKids = numKids + 1;
            j = j + 1; 
       end
       if numKids == 0
           treestr = '     /* epsilon */          { $$ = NULL; }';
       else
           treestr = [treestr, num2str(numKids), ', '];
           fprintf(fileID, '%s', tabstr);
           for k = 1:numKids
               currtoken =  char(curr_right{j - numKids + k - 1});
               if k ~= numKids
                    fprintf(fileID, '%s ', currtoken);
                    % if ~isempty(regexp(currtoken, '''', 'match'))
                    %     treestr = [treestr, 'NULL', ', '];
                    % elseif ~isempty(regexp(currtoken, '^[A-Z0-9_]+$', 'match'))
                    %     treestr = [treestr, 'NULL', ', '];
                    % else 
                        treestr = [treestr, ['$', num2str(k)], ', '];
                    % end
               else
                   % if ~isempty(regexp( currtoken, '''', 'match'))
                   %      treestr = [treestr, 'NULL', '); }'];
                   % elseif ~isempty(regexp( currtoken, '^[A-Z0-9_]+$', 'match'))
                   %      treestr = [treestr, 'NULL', '); }'];
                   % else 
                        treestr = [treestr,  ['$', num2str(k)], '); }'];
                   % end
                   fprintf(fileID, '%s ',  currtoken);
               end
            end
        end
       fprintf(fileID, '%s\n', treestr);
       semi_or_bar = regexp(char(curr_right{j}), '^[|;]$', 'match');
       if strcmp(semi_or_bar, '|')
           fprintf(fileID, '%s', '   |');
           tabstr = ' ';
       elseif strcmp(semi_or_bar, ';')
           fprintf(fileID, '%s\n\n', '   ;');
       end
       j = j + 1;
    end
    fprintf(fileID, '%s\n', '');
end

fclose(fileID);

