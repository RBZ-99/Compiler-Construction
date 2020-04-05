#include "codegen.h"
#include <string.h>
#include <stdlib.h>

void assign_next_label(tree_node *ast_node)
{
    if(ast_node->sibling){
        ast_node->label.next_label = newlabel();                            
        // code_emit(LABEL_OP, ast_node->label.next_label, NULL, NULL);
    }
    else{
        ast_node->label.next_label = ast_node->parent->label.next_label;        
        if(ast_node->label.next_label == NULL)
            ast_node->label.next_label = "function_end :";
    }
    printf("Next label is assigned as %s\n", ast_node->label.next_label);
}

void label(char *lbl){
    to_be_attached_label = lbl;
}

void print_quadruples()
{
    printf("%-15s | ","operator");
    printf("%-15s | ","arg1");
    printf("%-15s | ","arg2");
    printf("%-15s | ","result");
    printf("\n");
    printf("%-15s | ","---------------");
    printf("%-15s | ","---------------");
    printf("%-15s | ","---------------");
    printf("%-15s | ","---------------");
    printf("\n\n");

    for(int i = 0; i < quad_count; i++){
        printf("%-15s | ", tac_op_str[quadruples[i].op] );
        if(quadruples[i].arg1){
            printf("%-15s | ", quadruples[i].arg1);
        }
        else
            printf("%-15s | ","(empty)");
        if(quadruples[i].arg2){
            printf("%-15s | ", quadruples[i].arg2);
        }
        else
            printf("%-15s | ","(empty)");
        if(quadruples[i].result){
            printf("%-15s | ", quadruples[i].result);
        }
        else
            printf("%-15s | ","(empty)");
        printf("\n");
    }
}

void codegen_init()
{
    label_count = 0;
    temp_count = 0;
    quad_count = 0;
    switch_tbl_entry_num = 0;
    char *tac_op_str_tmp[NUM_TAC_OP] = {"+", "-", "*", "/", ">=", ">", "<=", "<", "==", "!=", "&&",
                                        "||", "uminus", "label","input", "output", "assign", "jmp", "jmp_if_true", "jmp_if_false",
        "param" ,"call", "indexed_copy", "array_access", "return", "function"};
    for(int i=0; i<NUM_TAC_OP; i++){        
        strcpy(tac_op_str[i], tac_op_str_tmp[i]);
    }

    to_be_attached_label = NULL;
}

char *newlabel()
{
    char *label = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
    sprintf(label, "label_%d :", label_count);
    printf("Emitted label %s\n", label);
    label_count++;
    return label;
}

tree_node *newtemp(tree_node *expr1_node, operator op, tree_node *expr2_node)
{
    // printf("Called newtemp()\n");
    tree_node *tmp_node = create_tree_node();        
    char *tmp_str = (char*)malloc(sizeof(char) * MAX_TEMP_LEN);
    sprintf(tmp_str, "temp_%d", temp_count);
    temp_count++;
    
    tmp_node->addr = tmp_str;
    tmp_node->encl_fun_type_ptr = expr1_node->encl_fun_type_ptr;
    tmp_node->scope_sym_tab = expr1_node->scope_sym_tab;
    tmp_node->sym.is_terminal = true;
    tmp_node->sym.t = ID;
    strcpy( tmp_node->token.id.str, tmp_str);
    tmp_node->token.name = ID;
    
    type *tmp_type, *e1_type, *e2_type, *e_type;
    e1_type = create_type();
    e2_type = create_type();
    e_type = create_type();
    *e1_type = get_expr_type(expr1_node, expr1_node->scope_sym_tab);
    if(expr2_node != NULL){
        *e2_type = get_expr_type(expr2_node, expr2_node->scope_sym_tab);
        *e_type = get_EopE_type(*e1_type, op, *e2_type);
        tmp_type = e_type;
    }
    else{
        tmp_type = e1_type;
    }
    // printf("e1_type->name = %s\n", terminal_string[e1_type->name]);
    switch(e1_type->name){
        case BOOLEAN:
            tmp_type->width = WIDTH_BOOLEAN;
        break;
        case INTEGER:
            tmp_type->width = WIDTH_INTEGER;
        break;
        case REAL:
            tmp_type->width = WIDTH_REAL;
        break;        
        default:
            tmp_type->width = 0;
        break;
    }
    // printf("width allocated to tmp : %d\n", tmp_type->width);
    if(tmp_node->encl_fun_type_ptr == NULL){
        printf("encl_fun_type_ptr for tmp is NULL\n\n");
    }
    tmp_type->offset = tmp_node->encl_fun_type_ptr->typeinfo.module.curr_offset;
    tmp_node->encl_fun_type_ptr->typeinfo.module.curr_offset += tmp_type->width;
    hash_insert_ptr_val(tmp_node->scope_sym_tab->table, tmp_node->token.id.str, tmp_type);
    return tmp_node;
}

char *to_string(tree_node *node)
{
    char *str_val = (char *)malloc( sizeof(char) * MAX_SYMBOL_LENGTH);
    switch(node->token.name){
        case ID:        
            strcpy(str_val, node->token.id.str);
        break;

        case NUM:
            sprintf(str_val, "#%d", node->token.num);
        break;

        case RNUM:
            sprintf(str_val, "#%lf", node->token.rnum);
        break;
        case TRUE:
        case FALSE:
            sprintf(str_val,"#%s", node->token.id.str);
        break;
        default:
            strcpy(str_val, "");
        break;
    }

    return str_val;
}

tac_op get_tac_op(tree_node *node){
    token_name t = node->token.name;
    tac_op op;
    switch(t){
        case PLUS:
            op = PLUS_OP;
        break;
        case MINUS:
            op = MINUS_OP;
        break;
        case MUL:
            op = MUL_OP;
        break;
        case DIV:
            op = DIV_OP;
        break;
        case GT:
            op = GT_OP;
        break;
        case GE:
            op = GE_OP;
        break;
        case LT:
            op = LT_OP;
        break;
        case LE:
            op = LE_OP;
        break;
        case NE:
            op = NE_OP;
        break;
        case EQ:
            op = EQ_OP;
        break;
        case AND:
            op = AND_OP;
        break;
        case OR:
            op = OR_OP;
        break;
        case GET_VALUE:
            op = INPUT_OP;
        break;
        case PRINT:
            op = OUTPUT_OP;
        break;            
    }
    return op;
}

void code_emit(tac_op op, char *arg1, char *arg2, char *result)
{
    if(op == LABEL_OP && arg1 == NULL)
        return;
    if(arg1){
        printf("inserting , arg1 : %s, ", arg1);
    }
    if(arg2){
        printf("inserting , arg2 : %s, ", arg2);
    }
    if(result){
        printf("inserting , result : %s", result);
    }
    printf("\n");
    if(to_be_attached_label != NULL){
        printf("to_be_attached_label : %s\n", to_be_attached_label);
        char *str = to_be_attached_label;
        to_be_attached_label = NULL;
        code_emit(LABEL_OP, str, NULL, NULL);        
    }
    quadruples[quad_count].op = op;
    quadruples[quad_count].arg1 = arg1;
    quadruples[quad_count].arg2 = arg2;
    quadruples[quad_count].result = result;
    quad_count++;
}

void generate_ir(tree_node *ast_node)
{
    while(ast_node != NULL){

        if(ast_node->visited == false){
            // printf("Visiting first time: ");
            // print_symbol_(ast_node);
            ast_node->visited = true;
            
            if(ast_node->sym.is_terminal == false){
                switch(ast_node->sym.nt){
                    case NTMODULE:
                    case DRIVERMODULE:
                    {                        
                        // assign_next_label(ast_node);
                        char *fun_name;
                        fun_name = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
                        strcpy(fun_name, ast_node->encl_fun_type_ptr->typeinfo.module.module_name);
                        strcat(fun_name, " :");
                        code_emit(FN_DEFN_OP, fun_name, NULL, NULL);                        
                        // printf("Assigning module label : %s\n", fun_name);
                        // label(fun_name);                        
                    }
                    break;     
                    case WHILELOOP:
                    {
                        char *begin = newlabel();
                        tree_node *expr_node = ast_node->leftmost_child;
                        tree_node *stmts_node = ast_node->rightmost_child;
                        expr_node->label.boolean.true_label = newlabel();
                        assign_next_label(ast_node);
                        expr_node->label.boolean.false_label = ast_node->label.next_label;
                        stmts_node->label.next_label = begin;
                        ast_node->label.cnstrct_code_begin = begin;
                        code_emit(LABEL_OP, begin, NULL, NULL);
                    }   
                    break;         
                    case ASSIGNMENTSTMT:
                    case IOSTMT:
                    case FUNCTIONCALLSTMT:
                    {
                        assign_next_label(ast_node);
                    }
                    break;
                    case FORLOOP:
                    {
                        assign_next_label(ast_node);
                        tree_node *iter_var_node = get_nth_child(ast_node, 1);
                        tree_node *range_low_node = get_nth_child(ast_node, 2);
                        tree_node *range_high_node = get_nth_child(ast_node, 3);

                        code_emit(ASSIGN_OP, to_string(iter_var_node), NULL,  to_string(range_low_node));
                        tree_node *tmp_node = newtemp(iter_var_node, NO_MATCHING_OP, NULL);
                        ast_node->label.cnstrct_code_begin = newlabel();
                        code_emit(LABEL_OP, ast_node->label.cnstrct_code_begin, NULL, NULL);
                        code_emit(LT_OP, to_string(iter_var_node), to_string(range_high_node), tmp_node->addr);
                        char *condn_true_lbl = newlabel();
                        code_emit(IF_TRUE_GOTO_OP, to_string(tmp_node), NULL, condn_true_lbl);
                        code_emit(GOTO_UNCOND_OP, ast_node->label.next_label, NULL, NULL);
                        code_emit(LABEL_OP, condn_true_lbl, NULL, NULL);                        
                    }
                    break;
                    case CONDITIONALSTMT:
                    {
                        assign_next_label(ast_node);
                        if(switch_test_label)
                            free(switch_test_label);
                        switch_test_label = newlabel();
                        code_emit(GOTO_UNCOND_OP, switch_test_label, NULL, NULL);
                        switch_tbl_entry_num = 0;
                    }
                    break;
                    case CASESTMT:
                    {
                        ast_node->label.cnstrct_code_begin = newlabel();
                        code_emit(LABEL_OP, ast_node->label.cnstrct_code_begin, NULL, NULL);
                    }
                    break;
                    case DEFAULTSTMT:
                    {
                        ast_node->label.cnstrct_code_begin = newlabel();
                        code_emit(LABEL_OP, ast_node->label.cnstrct_code_begin, NULL, NULL);
                    }
                    break;
                }
            }

            if(ast_node->leftmost_child != NULL){
                ast_node = ast_node->leftmost_child;
            }
        }
        else{
            ast_node->visited = false;
            // printf("Visiting second time: ");
            // print_symbol_(ast_node);
            if(ast_node->sym.is_terminal == false){
                switch(ast_node->sym.nt){
                    case TERM:                    
                    {                        
                        // printf("Created new temp for term/relationalexpr\n");
                        int num_child = ast_node->num_child;
                        if(num_child == 1){
                            ast_node->addr = ast_node->leftmost_child->addr;
                        }
                        else if(num_child == 3){                            
                            tree_node *first_child, *second_child, *third_child;
                            first_child = get_nth_child(ast_node, 1);
                            second_child = get_nth_child(ast_node, 2);
                            third_child = get_nth_child(ast_node, 3);
                            tree_node *addr_node = newtemp(first_child, get_operator(second_child), third_child);
                            ast_node->addr = addr_node->addr;
                            code_emit(get_tac_op(second_child), first_child->addr,third_child->addr, ast_node->addr);
                        }
                        if(ast_node->parent->sym.nt == WHILELOOP){
                            printf("leacving term and part of while\n");
                            code_emit(IF_TRUE_GOTO_OP, ast_node->addr, ast_node->label.boolean.true_label, NULL);
                            code_emit(GOTO_UNCOND_OP, ast_node->label.boolean.false_label, NULL, NULL);
                            code_emit(LABEL_OP, ast_node->label.boolean.true_label, NULL, NULL);
                        }
                    }
                    break;
                    case RELATIONALEXPR:
                    {                        
                        tree_node *first_child, *second_child, *third_child;
                        first_child = get_nth_child(ast_node, 1);
                        second_child = get_nth_child(ast_node, 2);
                        third_child = get_nth_child(ast_node, 3);
                        tree_node *addr_node = newtemp(first_child, get_operator(second_child), third_child);
                        ast_node->addr = addr_node->addr;
                        code_emit(get_tac_op(second_child), first_child->addr,third_child->addr, ast_node->addr);
                        // code_emit(GOTO_UNCOND_OP, ast_node->label.boolean.false_label, NULL, NULL);
                        if(ast_node->parent->sym.nt == WHILELOOP){
                            code_emit(IF_TRUE_GOTO_OP, ast_node->addr, ast_node->label.boolean.true_label, NULL);
                            code_emit(GOTO_UNCOND_OP, ast_node->label.boolean.false_label, NULL, NULL);
                            code_emit(LABEL_OP, ast_node->label.boolean.true_label, NULL, NULL);
                        }
                    }
                    break;
                    case EXPRESSION:
                    case NONUNARYARITHMETICEXPR:
                    {
                        int num_child = ast_node->num_child;
                        int num_loop_iter = (num_child + 1) / 2;
                        char *temp, *prev_temp;
                        tree_node *child = ast_node->leftmost_child;
                        tree_node *operand1, *operand2, *operator_node;
                        operand1 = child;
                        operator_node = child->sibling;
                        operand2 = operator_node->sibling;
                        tree_node *tmp_node;

                        for(int i = 0; i < num_loop_iter && operator_node != NULL; i++)
                        {                            
                            // printf("Created new temp for expression/nonunary\n");
                            operator op = get_operator(operator_node);
                            tmp_node = newtemp(operand1, op, operand2);
                            temp = tmp_node->addr;                             
                            
                            if(i == 0){                                                               
                                code_emit(get_tac_op(operator_node), operand1->addr, operand2->addr, temp);
                            }
                            else{
                                code_emit(get_tac_op(operator_node), prev_temp, operand2->addr, temp);
                            }
                            operand1 = operand2;
                            operator_node = operand2->sibling;
                            if(operator_node)
                                operand2 = operator_node->sibling;
                            prev_temp = temp;
                        }
                        ast_node->addr = temp;
                        if(ast_node->parent->sym.nt == WHILELOOP){
                            printf("leacving expr & nonunary and part of while\n");
                            code_emit(IF_TRUE_GOTO_OP, ast_node->addr, ast_node->label.boolean.true_label, NULL);
                            code_emit(GOTO_UNCOND_OP, ast_node->label.boolean.false_label, NULL, NULL);
                            code_emit(LABEL_OP, ast_node->label.boolean.true_label, NULL, NULL);
                        }
                    }
                    break;
                    case UNARYARITHMETICEXPR:
                    {                           
                        // printf("Created new temp for unaryarithmetic\n");
                        tree_node *expr_node = ast_node->rightmost_child;
                        tree_node *tmp_node = newtemp(expr_node, NO_MATCHING_OP, NULL);
                        ast_node->addr = tmp_node->addr;
                        /** 
                         * ToDo : it could be UPLUS also, correct it
                         */
                        code_emit(UMINUS_OP, expr_node->addr,NULL, ast_node->addr);
                    }
                    break;
                    case VAR:
                    {                        
                        tree_node *id_node = ast_node->leftmost_child;
                        if(id_node->sibling->sym.t != EPSILON){
                            tree_node *index_node = newtemp(id_node->sibling, NO_MATCHING_OP, NULL);
                            char *index = index_node->addr;
                            // printf("Created new temp for var\n");
                            type *arr_type = (type*)key_search_recursive(id_node->scope_sym_tab, id_node->token.id.str, id_node->encl_fun_type_ptr, NULL);
                            char *width = (char*) malloc(sizeof(char) * MAX_LABEL_LEN);
                            snprintf(width, MAX_LABEL_LEN, "#%d", arr_type->width);
                            code_emit(MUL_OP,id_node->sibling->addr, width, index);

                            tree_node *tmp_node = newtemp(id_node, NO_MATCHING_OP, NULL);
                            ast_node->addr = tmp_node->addr;

                            code_emit(ARRAY_ACCESS_OP, id_node->addr, index, ast_node->addr);
                        }
                        else{
                            ast_node->addr = id_node->addr;
                        }
                    }
                    break;
                    case ASSIGNMENTSTMT:
                    {
                        tree_node *rvalue_node = ast_node->rightmost_child;
                        tree_node *lvalue_id_node = ast_node->leftmost_child;
                        tree_node *index_node = lvalue_id_node->sibling;
                        char *lvalue;
                        if(index_node->sym.t == EPSILON)
                            code_emit(ASSIGN_OP, rvalue_node->addr, NULL, lvalue_id_node->addr);
                        else
                            code_emit(INDEXED_COPY_OP,index_node->addr, rvalue_node->addr, lvalue_id_node->addr);

                        
                        // ast_node->label.next_label = newlabel();                            
                        // label(ast_node->label.next_label);
                        if(strcmp(ast_node->label.next_label, "function_end :"))
                            code_emit(LABEL_OP, ast_node->label.next_label, NULL, NULL);
                        
                    }
                    break;
                    case IOSTMT:
                    {
                        tree_node *action_node = ast_node->leftmost_child;                        
                        if(action_node->token.name == GET_VALUE){
                            tree_node *id_node = ast_node->rightmost_child;
                            code_emit(INPUT_OP, id_node->token.id.str, NULL, NULL);
                        }
                        else{   //print
                            tree_node *var_node = ast_node->rightmost_child;
                            code_emit(OUTPUT_OP, var_node->addr, NULL, NULL);
                        }
                        
                        // ast_node->label.next_label = newlabel();                            
                        // label(ast_node->label.next_label);
                        if(strcmp(ast_node->label.next_label, "function_end :"))
                            code_emit(LABEL_OP, ast_node->label.next_label, NULL, NULL);
                    }
                    break;
                    case FUNCTIONCALLSTMT:
                    {
                        tree_node *inp_params_node = ast_node->rightmost_child;
                        int param_num = inp_params_node->num_child;
                        tree_node *param = inp_params_node->leftmost_child;
                        for(int i = 0; i < param_num; i++){
                            code_emit(PARAM_OP, param->addr, NULL, NULL);
                            param = param->sibling;
                        }
                        char *param_num_str = (char*)malloc(sizeof(char) * MAX_LABEL_LEN);
                        snprintf(param_num_str, MAX_LABEL_LEN, "%d", param_num);
                        code_emit(PROC_CALL_OP, get_nth_child(ast_node, 2)->addr, param_num_str, NULL);
                        
                        // ast_node->label.next_label = newlabel();                            
                        // label(ast_node->label.next_label);
                        if(strcmp(ast_node->label.next_label, "function_end :"))
                            code_emit(LABEL_OP, ast_node->label.next_label, NULL, NULL);
                        
                    }
                    break;
                    case WHILELOOP:
                    {
                        code_emit(GOTO_UNCOND_OP, ast_node->label.cnstrct_code_begin, NULL, NULL);
                        if(strcmp(ast_node->label.next_label, "function_end :"))
                            code_emit(LABEL_OP, ast_node->label.next_label, NULL, NULL);
                        printf("And whileloop second time visiting emits after jmp begin, label %s\n", ast_node->label.next_label);
                        // ast_node->label.next_label = newlabel();                            
                        // label(ast_node->label.next_label);                        
                    }
                    break;
                    case FORLOOP:
                    {
                        /**
                         * @brief Increment iterating variable
                         */
                        
                        tree_node *iter_node = get_nth_child(ast_node, 1);
                        code_emit(PLUS_OP, iter_node->addr, "#1", iter_node->addr);
                        code_emit(GOTO_UNCOND_OP, ast_node->label.cnstrct_code_begin, NULL, NULL);

                        if(strcmp(ast_node->label.next_label, "function_end :"))
                            code_emit(LABEL_OP, ast_node->label.next_label, NULL, NULL);
                        // ast_node->label.next_label = newlabel();                            
                        // label(ast_node->label.next_label);
                    }
                    break;
                    case CONDITIONALSTMT:
                    {
                        tree_node *switch_var_node = ast_node->leftmost_child;
                        tree_node *tmp_var = newtemp(switch_var_node, NO_MATCHING_OP, NULL);
                        int num_cases = get_nth_child(ast_node, 2)->num_child;

                        /**
                         * @brief Emit test label
                         */

                        code_emit(LABEL_OP, switch_test_label, NULL, NULL);

                        for(int i=0; i<num_cases; i++){
                            code_emit(EQ_OP, switch_table[i].value, switch_var_node->addr, tmp_var->addr);
                            code_emit(IF_TRUE_GOTO_OP, tmp_var->addr, switch_table[i].label, NULL);
                        }

                        tree_node *default_node = ast_node->rightmost_child;
                        if(default_node->sym.is_terminal == false){
                            code_emit(GOTO_UNCOND_OP, switch_table[switch_tbl_entry_num-1].label, NULL, NULL);
                        }

                        if(strcmp(ast_node->label.next_label, "function_end :"))
                            code_emit(LABEL_OP, ast_node->label.next_label, NULL, NULL);
                        // ast_node->label.next_label = newlabel();                            
                        // label(ast_node->label.next_label);
                    }
                    break;
                    case NTMODULE:
                    case DRIVERMODULE:
                    {
                        // ast_node->label.next_label = newlabel();                            
                        // label(ast_node->label.next_label);
                        code_emit(LABEL_OP, "function_end :", NULL, NULL);                        
                        code_emit(RETURN_OP, NULL, NULL, NULL);                        
                    }
                    break;
                    case CASESTMT:
                    {
                        tree_node *switch_node = ast_node->parent->parent;
                        code_emit(GOTO_UNCOND_OP, switch_node->label.next_label, NULL, NULL);
                        
                        switch_table[switch_tbl_entry_num].label = ast_node->label.cnstrct_code_begin;
                        switch_table[switch_tbl_entry_num].value = ast_node->leftmost_child->addr;
                        switch_tbl_entry_num++;
                    }
                    break;
                    case DEFAULTSTMT:
                    {
                        tree_node *switch_node = ast_node->parent->parent;
                        code_emit(GOTO_UNCOND_OP, switch_node->label.next_label, NULL, NULL);
                        
                        switch_table[switch_tbl_entry_num].label = ast_node->label.cnstrct_code_begin;
                        switch_table[switch_tbl_entry_num].value = "default_case";
                        switch_tbl_entry_num++;
                    }
                    break;
                }
            }
            else{
                switch(ast_node->sym.t){
                    case ID:                    
                    case RNUM:                        
                    case NUM:
                    case TRUE:
                    case FALSE:    
                    {
                        ast_node->addr = to_string(ast_node);
                    }                    
                    break;
                }
            }

            if(ast_node->sibling){
                ast_node = ast_node->sibling;
            }
            else{
                ast_node = ast_node->parent;
            }
        }

    }
}