#!/usr/bin/env python3
"""Generate a compact, button-driven effect preview for every demo panel."""
import json
from pathlib import Path
BASE=Path(__file__).parent
for w,h in [(240,240),(320,240),(360,360),(480,800),(800,480),(1024,600)]:
    scale=min(w/320,h/320,1.7);ox=(w-320*scale)/2;oy=(h-300*scale)/2
    objects=[]
    def add(kind,x,y,width,height,parent=-1,**kw):
        item=dict(type=kind,parent=parent,x=round(x*scale+(ox if parent==-1 else 0)),y=round(y*scale+(oy if parent==-1 else 0)),w=round(width*scale),h=round(height*scale),**kw)
        if kind=='container' and 'name' in kw:item.update(bind=kw['name']+'_visible',bind_target='visible')
        if 'font_size' in item:item['font_size']=max(10,round(item['font_size']*scale))
        objects.append(item);return len(objects)-1
    add('label',40,7,240,24,name='title',bind='title_text',bind_target='text',text='Charging orb',font_charset='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz /',fg_color='#E8F4FF',font_size=18,text_align='center')
    for page_name in ['charge','ring','orbit','pulse','morph','shimmer','feedback','flip','glass']:
        page=add('container',0,35,320,184,name=page_name+'_page',hidden=page_name!='charge')
        if page_name=='charge':
            for i,style in enumerate(['liquid','halo','plasma']):
                variant=add('container',0,0,320,184,page,name='charge_'+style+'_view',hidden=i!=1)
                add('charging_orb',70,0,180,180,variant,name='charge_'+style,style=style,value=62,fg_color='#EDB1C3' if style=='plasma' else '#5BCFF5',strength=85,softness=90,ripple=55,period_ms=3600)
            add('label',90,65,140,42,page,name='level',bind='level_text',bind_target='text',text='62%',font_charset='0123456789.%',fg_color='#F0FFFF',font_size=34,text_align='center')
            add('label',100,110,120,20,page,name='charge_status',bind='charge_status_text',bind_target='text',font_charset='ABCDEFGHIJKLMNOPQRSTUVWXYZ ',text='CHARGING',fg_color='#91B8C8',font_size=10,text_align='center')
        elif page_name=='ring':
            for i,style in enumerate(['solid','glow','trail']):
                variant=add('container',0,0,320,184,page,name='ring_'+style+'_view',hidden=i!=2)
                add('effect',82,8,156,156,variant,name='ring_'+style,effect='ring',ring_style=style,value=75,fg_color='#78DBFA',strength=85,softness=90,period_ms=2400)
        elif page_name=='orbit':
            for i,mode in enumerate(['flat','tilt','perspective']):
                variant=add('container',0,0,320,184,page,name='orbit_'+mode+'_view',hidden=i!=2)
                add('carousel',16,0,288,178,variant,name='menu_'+mode,icons=['../assets/widgets/carousel/'+n+'.svg' for n in ['home','music','water','sun','power']],projection=mode,depth=60,spacing=45,callback='selected')
        elif page_name=='pulse':
            add('effect',90,14,140,140,page,name='pulse',effect='pulse',runtime_style=True,fg_color='#9CBDFB',strength=70,softness=90,period_ms=3600)
        elif page_name=='morph':
            add('image',104,24,112,112,page,name='emblem',image='../assets/widgets/image/vector_emblem.svg',svg_element='emblem',morph_to='../assets/widgets/image/vector_emblem_active.svg',morph=0,fit='contain')
        elif page_name=='shimmer':
            for i,mode in enumerate(['linear','soft','diagonal']):
                variant=add('container',0,0,320,184,page,name='shimmer_'+mode+'_view',hidden=i!=1)
                add('rect',38,53,244,66,variant,radius=14,bg_color='#173B57')
                add('effect',46,65,228,42,variant,name='shine_'+mode,effect='shimmer',shimmer_style=mode,fg_color='#C7F7FF',strength=65,period_ms=2200)
                add('label',68,77,184,24,variant,text='POWERING UP',font_size=15,fg_color='#F0FFFF',text_align='center')
        elif page_name=='feedback':
            add('effect',50,-22,220,220,page,name='feedback',effect='pulse',runtime_style=True,playing=False,value=0,fg_color='#7AACFF',strength=100,softness=95)
            add('button',86,65,148,48,page,name='touch_target',text='TOUCH',font_size=16,bg_color='#244766',fg_color='#E8FAFF',radius=12,events=[dict(event='click',action='play_glow',target_name='feedback')])
            add('label',45,140,230,20,page,text='Tap to light up',font_size=12,fg_color='#A3BAC9',text_align='center')
        elif page_name=='flip':
            add('flip_card',35,0,250,178,page,name='card',runtime_style=True,front='../assets/card_front.svg',back='../assets/card_back.svg',image_sets=[['../assets/card_front_teal.svg','../assets/card_back_teal.svg']],depth=45,period_ms=600)
        elif page_name=='glass':
            for i,(mode,blur,tint,amount) in enumerate([('mist',6,'#CCE8FF',30),('frost',14,'#D8EAFF',65),('smoke',20,'#0B1D38',130)]):
                variant=add('container',0,0,320,184,page,name='glass_'+mode+'_view',hidden=i!=1)
                add('image',0,0,320,184,variant,name='backdrop_'+mode,image='../assets/backdrop.svg',fit='stretch')
                add('glass',48,23,224,138,variant,name='glass_'+mode,backdrop='backdrop_'+mode,blur=blur,tint_color=tint,tint_opacity=amount,radius=16)
                add('label',70,45,180,20,variant,text='LIVING ROOM',font_size=12,fg_color='#F1F7FF',text_align='center')
                add('label',70,70,180,42,variant,text='22.6 C',font_size=32,fg_color='#FFFFFF',text_align='center')
                add('label',70,122,180,18,variant,text='Comfortable',font_size=11,fg_color='#E3EDF6',text_align='center')
    for i,label in enumerate(['Liquid','Halo','Plasma']):
        add('button',35+i*87,223,76,26,name='style_'+str(i),bind='style_text_'+str(i),bind_target='text',text=label,font_charset='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz',font_size=12,bg_color='#18304A',fg_color='#DDF4FF',radius=7,callback='style_'+str(i))
        add('container',43+i*87,251,60,2,name='mark_'+str(i),bind='mark_visible_'+str(i),bind_target='visible',hidden=i!=1,bg_color='#66D9FF')
    pause_view=add('container',75,258,78,27,name='pause_view')
    add('button',0,0,78,27,pause_view,name='pause',bind='pause_text',bind_target='text',font_charset='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz',text='Stop',font_size=12,bg_color='#18304A',fg_color='#DDF4FF',radius=8,callback='pause')
    add('button',167,258,78,27,name='next',text='Next',font_size=12,bg_color='#285B85',fg_color='#FFFFFF',radius=8,callback='next')
    (BASE/f'effects_{w}.json').write_text(json.dumps(dict(screen='effects',w=w,h=h,screen_bg='#08131F',font='../fonts/DejaVuSans.ttf',objects=objects),indent=2)+'\n')
