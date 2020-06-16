
#define no_of_directive_strings 21
struct directive_id directive_list[] = {
   "manufacturer", manufacturer,
   "major_model", major_model,
   "minor_model", minor_model,
   "year", year,
   "page", page,
   "title", title,
   "variant_start", variant_start,
   "variant", variant,
   "variant_end", variant_end,
   "variant_quantity", variant_quantity,
   "variant_quantity_end", variant_quantity_end,
   "variant_assembly_start", variant_assembly_start,
   "variant_assembly", variant_assembly,
   "in_assembly", in_assembly,
   "common_assembly", common_assembly,
   "variant_assembly_end", variant_assembly_end,
   "variant_one_of_set",variant_one_of_set,
   "end_variant_set",end_variant_set,
   "publication_part_no",publication_part_no,
   "engine_number",engine_number,
   "end",end
   };

#define no_of_alt_part_manufacturers 15
static char alt_manufacturer_list [no_of_alt_part_manufacturers][12] = {
   "Amal",
   "Lucas",
   "Girling",
   "AP",
   "Champion",
   "Smiths",
   "Merit-Worke",
   "Veglia",
   "Borg-Beck",
   "Siba",
   "Clearhooter",
   "BTH",
   "Lodge",
   "KLG",
   "Zenith"
   };

#define part_number_prefixes 9
static char alpha_to_num[part_number_prefixes][2][4] = {
   {"D", "60"},
   {"E", "70"},
   {"E1", "71"},
   {"F", "82"},
   {"F1", "83"},
   {"H", "97"},
   {"S", "21"},
   {"T", "57"},
   {"W", "37"}
   };

#define number_of_odd_parts 66
static char odd_parts[number_of_odd_parts][2][10] = {
   {"A3", "60-0003"},
   {"A40", "60-0778"},
   {"A44", "60-0044"},
   {"A52", "60-0052"},
   {"A56", "60-0056"},
   {"A61", "60-0061"},
   {"D208-5", "60-4255"},
   {"D343-7", "60-4258"},
   {"E1719A", "71-3749"},
   {"GS299", "60-4256"},
   {"GS299A", "60-4257"},
   {"GS511", "60-4249"},
   {"NW236", "37-4199"},
   {"PCW73A", "60-4251"},
   {"S25-1", "60-4245"},
   {"S25-2", "60-4246"},
   {"S25-3", "60-4247"},
   {"S25-13", "60-4248"},
   {"S26-2", "60-4250"},
   {"S66-1", "60-4253"},
   {"S79-2", "37-4200"},
   {"T423A", "57-4754"},
   {"WE259A", "71-3748"},
   {"WF32", "60-4252"},
   {"XA69", "60-4254"},
   {"E3059R","-"},
   {"T24A","-"},
   {"WE170","-"},
   {"D343-3","-"},
   {"GS251","-"},          /* From T150 '69 */
   {"GS308","-"},
   {"S25-43","-"},
   {"T47","57-0047"},
   {"T57","57-0057"},
   {"T23","57-0023"},
   {"T408/9","-"},
   {"T31","57-0031"},
   {"S70-3","-"},
   {"NT261","-"},
   {"T44","-"},
   {"T260B","-"},
   {"S70-3","-"},
   {"S26-1","-"},
   {"S26-3","-"},
   {"NW100","-"},
   {"E410A","-"},
   {"H519A","-"},
   {"H95","-"},
   {"S25-6","-"},
   {"W270A","-"},
   {"NW243","-"},
   {"S1-52","-"},
   {"WM3","-"},
   {"57160","-"},  /* Oil cooler reflector */
   {"S25-71","-"},
   {"DS73","-"},
   {"D83","-"},
   {"NS191","-"},
   {"D44","-"},
   {"CP212","-"},
   {"CP21210","-"},
   {"CP21220","-"},
   {"CP21240","-"},
   {"T45","-"},   	  /* From T150 '73 */
   {"199001","-"},
   {"M10441","-"}
   };

char variant_symbol[5] = {
   ')','>','}',']','#'};
