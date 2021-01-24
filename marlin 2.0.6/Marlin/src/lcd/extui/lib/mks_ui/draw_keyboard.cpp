/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "../../../../inc/MarlinConfigPre.h"

#if HAS_TFT_LVGL_UI

#include "lv_conf.h"
#include "draw_ui.h"

#include "../../../../../Configuration.h"
#include "../../../../MarlinCore.h"

extern lv_group_t * g;
static lv_obj_t * scr;

#define LV_KB_CTRL_BTN_FLAGS (LV_BTNM_CTRL_NO_REPEAT | LV_BTNM_CTRL_CLICK_TRIG)

static const char * kb_map_lc[] = {"1#", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", LV_SYMBOL_BACKSPACE, "\n",
                                   "ABC", "a", "s", "d", "f", "g", "h", "j", "k", "l", LV_SYMBOL_NEW_LINE, "\n",
                                   "_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
                                   LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""};

static const lv_btnm_ctrl_t kb_ctrl_lc_map[] = {
    LV_KB_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KB_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KB_CTRL_BTN_FLAGS | 2};

static const char * kb_map_uc[] = {"1#", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", LV_SYMBOL_BACKSPACE, "\n",
                                   "abc", "A", "S", "D", "F", "G", "H", "J", "K", "L", LV_SYMBOL_NEW_LINE, "\n",
                                   "_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
                                   LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""};

static const lv_btnm_ctrl_t kb_ctrl_uc_map[] = {
    LV_KB_CTRL_BTN_FLAGS | 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7,
    LV_KB_CTRL_BTN_FLAGS | 6, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KB_CTRL_BTN_FLAGS | 2};

static const char * kb_map_spec[] = {"0", "1", "2", "3", "4" ,"5", "6", "7", "8", "9", LV_SYMBOL_BACKSPACE, "\n",
                                     "abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
                                     "\\",  "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
                                     LV_SYMBOL_CLOSE, LV_SYMBOL_LEFT, " ", LV_SYMBOL_RIGHT, LV_SYMBOL_OK, ""};

static const lv_btnm_ctrl_t kb_ctrl_spec_map[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
    LV_KB_CTRL_BTN_FLAGS | 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    LV_KB_CTRL_BTN_FLAGS | 2, 2, 6, 2, LV_KB_CTRL_BTN_FLAGS | 2};

static const lv_btnm_ctrl_t kb_ctrl_num_map[] = {
        1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
        1, 1, 1, LV_KB_CTRL_BTN_FLAGS | 2,
        1, 1, 1, 2,
        1, 1, 1, 1, 1};

static void lv_kb_event_cb(lv_obj_t * kb, lv_event_t event) {
    //LV_ASSERT_OBJ(kb, LV_OBJX_NAME);

    if(event != LV_EVENT_VALUE_CHANGED) return;

    lv_kb_ext_t * ext = (lv_kb_ext_t * )lv_obj_get_ext_attr(kb);
    uint16_t btn_id   = lv_btnm_get_active_btn(kb);
    if(btn_id == LV_BTNM_BTN_NONE) return;
    if(lv_btnm_get_btn_ctrl(kb, btn_id, LV_BTNM_CTRL_HIDDEN | LV_BTNM_CTRL_INACTIVE)) return;
    if(lv_btnm_get_btn_ctrl(kb, btn_id, LV_BTNM_CTRL_NO_REPEAT) && event == LV_EVENT_LONG_PRESSED_REPEAT) return;

    const char * txt = lv_btnm_get_active_btn_text(kb);
    if(txt == NULL) return;

    /*Do the corresponding action according to the text of the button*/
    if(strcmp(txt, "abc") == 0) {
        lv_btnm_set_map(kb, kb_map_lc);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_lc_map);
        return;
    } else if(strcmp(txt, "ABC") == 0) {
        lv_btnm_set_map(kb, kb_map_uc);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_uc_map);
        return;
    } else if(strcmp(txt, "1#") == 0) {
        lv_btnm_set_map(kb, kb_map_spec);
        lv_btnm_set_ctrl_map(kb, kb_ctrl_spec_map);
        return;
    } else if(strcmp(txt, LV_SYMBOL_CLOSE) == 0) {
        if(kb->event_cb != lv_kb_def_event_cb) {
            //lv_res_t res = lv_event_send(kb, LV_EVENT_CANCEL, NULL);
            //if(res != LV_RES_OK) return;
            lv_clear_keyboard();
			draw_return_ui();
        } else {
            lv_kb_set_ta(kb, NULL); /*De-assign the text area  to hide it cursor if needed*/
            lv_obj_del(kb);
            return;
        }
        return;
    } else if(strcmp(txt, LV_SYMBOL_OK) == 0) {
        if(kb->event_cb != lv_kb_def_event_cb) {
            //lv_res_t res = lv_event_send(kb, LV_EVENT_APPLY, NULL);
            //if(res != LV_RES_OK) return;
            const char * ret_ta_txt = lv_ta_get_text(ext->ta);
			switch(keyboard_value)
			{
                #if USE_WIFI_FUNCTION
                    case wifiName:
                        memcpy(uiCfg.wifi_name,ret_ta_txt,sizeof(uiCfg.wifi_name));
                        lv_clear_keyboard();
                        draw_return_ui();
                        break;
                    case wifiPassWord:
                        memcpy(uiCfg.wifi_key,ret_ta_txt,sizeof(uiCfg.wifi_name));
                        lv_clear_keyboard();
                        draw_return_ui();
                        break;
                    case wifiConfig:
                        memset((void *)uiCfg.wifi_name, 0, sizeof(uiCfg.wifi_name));
                        memcpy((void *)uiCfg.wifi_name, wifi_list.wifiName[wifi_list.nameIndex], 32);

                        memset((void *)uiCfg.wifi_key, 0, sizeof(uiCfg.wifi_key));
                        memcpy((void *)uiCfg.wifi_key, ret_ta_txt, sizeof(uiCfg.wifi_key));

                        gCfgItems.wifi_mode_sel = STA_MODEL;
                        
                        package_to_wifi(WIFI_PARA_SET, (char *)0, 0);

                        memset(public_buf_l,0,sizeof(public_buf_l));
                        
                        public_buf_l[0] = 0xA5;
                        public_buf_l[1] = 0x09;
                        public_buf_l[2] = 0x01;
                        public_buf_l[3] = 0x00;
                        public_buf_l[4] = 0x01;
                        public_buf_l[5] = 0xFC;
                        public_buf_l[6] = 0x00;
                        raw_send_to_wifi(public_buf_l, 6);

                        last_disp_state = KEY_BOARD_UI;
                        lv_clear_keyboard();
                        wifi_tips_type = TIPS_TYPE_JOINING;
                        lv_draw_wifi_tips();
                        break;
                #endif //USE_WIFI_FUNCTION
                case gcodeCommand:
                    uint8_t buf[100];
                    strncpy((char *)buf,ret_ta_txt,sizeof(buf));
                    update_gcode_command(AUTO_LEVELING_COMMAND_ADDR,buf);
                    lv_clear_keyboard();
					draw_return_ui();
                    break;
				default:
					break;
			}
			
        } else {
            lv_kb_set_ta(kb, NULL); /*De-assign the text area to hide it cursor if needed*/
        }
        return;
    }

    /*Add the characters to the text area if set*/
    if(ext->ta == NULL) return;

    if(strcmp(txt, "Enter") == 0 || strcmp(txt, LV_SYMBOL_NEW_LINE) == 0)
        lv_ta_add_char(ext->ta, '\n');
    else if(strcmp(txt, LV_SYMBOL_LEFT) == 0)
        lv_ta_cursor_left(ext->ta);
    else if(strcmp(txt, LV_SYMBOL_RIGHT) == 0)
        lv_ta_cursor_right(ext->ta);
    else if(strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
        lv_ta_del_char(ext->ta);
    else if(strcmp(txt, "+/-") == 0) {
        uint16_t cur        = lv_ta_get_cursor_pos(ext->ta);
        const char * ta_txt = lv_ta_get_text(ext->ta);
        if(ta_txt[0] == '-') {
            lv_ta_set_cursor_pos(ext->ta, 1);
            lv_ta_del_char(ext->ta);
            lv_ta_add_char(ext->ta, '+');
            lv_ta_set_cursor_pos(ext->ta, cur);
        } else if(ta_txt[0] == '+') {
            lv_ta_set_cursor_pos(ext->ta, 1);
            lv_ta_del_char(ext->ta);
            lv_ta_add_char(ext->ta, '-');
            lv_ta_set_cursor_pos(ext->ta, cur);
        } else {
            lv_ta_set_cursor_pos(ext->ta, 0);
            lv_ta_add_char(ext->ta, '-');
            lv_ta_set_cursor_pos(ext->ta, cur + 1);
        }
    } else {
        lv_ta_add_text(ext->ta, txt);
    }
}

void lv_draw_keyboard() {
	if(disp_state_stack._disp_state[disp_state_stack._disp_index] != KEY_BOARD_UI) {
		disp_state_stack._disp_index++;
		disp_state_stack._disp_state[disp_state_stack._disp_index] = KEY_BOARD_UI;
	}
	disp_state = KEY_BOARD_UI;

	scr = lv_obj_create(NULL, NULL);
	
	lv_obj_set_style(scr, &tft_style_scr);
  	lv_scr_load(scr);
  	lv_obj_clean(scr);

  	lv_refr_now(lv_refr_get_disp_refreshing());
	
    /*Create styles for the keyboard*/
    static lv_style_t rel_style, pr_style;

    lv_style_copy(&rel_style, &lv_style_btn_rel);
    rel_style.body.radius = 0;
    rel_style.body.border.width = 1;
	rel_style.body.main_color = lv_color_make(0xa9, 0x62, 0x1d);
    rel_style.body.grad_color = lv_color_make(0xa7, 0x59, 0x0e);

    lv_style_copy(&pr_style, &lv_style_btn_pr);
    pr_style.body.radius = 0;
    pr_style.body.border.width = 1;
	pr_style.body.main_color = lv_color_make(0x72, 0x42, 0x15);
    pr_style.body.grad_color = lv_color_make(0x6a, 0x3a, 0x0c);

    /*Create a keyboard and apply the styles*/
    lv_obj_t *kb = lv_kb_create(scr, NULL);
	lv_obj_set_event_cb(kb, lv_kb_event_cb);
    lv_kb_set_cursor_manage(kb, true);
    lv_kb_set_style(kb, LV_KB_STYLE_BG, &lv_style_transp_tight);
    lv_kb_set_style(kb, LV_KB_STYLE_BTN_REL, &rel_style);
    lv_kb_set_style(kb, LV_KB_STYLE_BTN_PR, &pr_style);
    #if BUTTONS_EXIST(EN1, EN2, ENC)
	if (gCfgItems.encoder_enable == true) {
		//lv_group_add_obj(g, kb);
        //lv_group_set_editing(g, true);
	}
    #endif // BUTTONS_EXIST(EN1, EN2, ENC)

    /*Create a text area. The keyboard will write here*/
    lv_obj_t *ta = lv_ta_create(scr, NULL);
    lv_obj_align(ta, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);
    if(keyboard_value == gcodeCommand) {
        get_gcode_command(AUTO_LEVELING_COMMAND_ADDR,(uint8_t *)public_buf_m);
        public_buf_m[sizeof(public_buf_m)-1] = 0;
        lv_ta_set_text(ta, public_buf_m);
    }
    else {
        lv_ta_set_text(ta, "");
    }
    

    /*Assign the text area to the keyboard*/
    lv_kb_set_ta(kb, ta);
}

void lv_clear_keyboard() { 
	#if BUTTONS_EXIST(EN1, EN2, ENC)
	if (gCfgItems.encoder_enable == true) {
		//lv_group_remove_all_objs(g);
	}
  	#endif // BUTTONS_EXIST(EN1, EN2, ENC)
	lv_obj_del(scr); 
}


#endif  // HAS_TFT_LVGL_UI
