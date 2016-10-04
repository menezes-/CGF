/*
 *  TextureManager.cpp
 *  framework
 *
 *  Created by Marcelo Cohen on 07/13.
 *  Copyright 2013 PUCRS. All rights reserved.
 *
 */

#include "TextureManager.h"
#include "Debug.h"

namespace cgf
{

using namespace std;

TextureManager TextureManager::m_TextureManager;

TextureManager::TextureManager()
{
    defaultImageDir = "data";
}

TextureManager::~TextureManager()
{
    //dtor
    DEBUG_MSG("~TextureManager: deleting " << imgs.size());
    map<string,sf::Texture*>::iterator it = imgs.begin();
    while(it != imgs.end())
    {
        sf::Texture* tex = (*it).second;
        delete tex;
        it++;
    }
}

sf::Texture* TextureManager::findTexture(const char *nomeArq)
{
    if(imgs.find(nomeArq) == imgs.end()) {
        DEBUG_MSG_NN("New texture: " << nomeArq);
        sf::Texture* tex = new sf::Texture();
        if(!tex->loadFromFile(nomeArq))
            return NULL;
        DEBUG_MSG(" (" << tex->getSize().x << " x " << tex->getSize().y << ")");
        imgs[nomeArq] = tex;
        tex->setSmooth(true);
        return tex;
    }
    // Return texture id
    DEBUG_MSG("Existing texture: " << nomeArq << " (" << imgs[nomeArq]->getSize().x << " x " << imgs[nomeArq]->getSize().y << ")");
    return imgs[nomeArq];
}

void TextureManager::setDefaultImageDir(const char *dir)
{
    defaultImageDir = dir;
}

void TextureManager::releaseTexture(const char *nomeArq)
{
    if(imgs.find(nomeArq) != imgs.end())
    {
        sf::Texture* tex = imgs[nomeArq];
        imgs.erase(nomeArq);
        delete tex;
        //glDeleteTextures(1, &tex);
    }
}

} // namespace cgf
